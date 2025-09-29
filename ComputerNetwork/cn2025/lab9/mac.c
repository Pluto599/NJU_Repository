#include "mac.h"
#include "log.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

mac_port_map_t mac_port_map;

// initialize mac_port table
void init_mac_port_table()
{
	bzero(&mac_port_map, sizeof(mac_port_map_t));

	for (int i = 0; i < HASH_8BITS; i++) {
		init_list_head(&mac_port_map.hash_table[i]);
	}

	pthread_mutex_init(&mac_port_map.lock, NULL);

	pthread_create(&mac_port_map.thread, NULL, sweeping_mac_port_thread, NULL);
}

// destroy mac_port table
void destory_mac_port_table()
{
	pthread_mutex_lock(&mac_port_map.lock);
	mac_port_entry_t *entry, *q;
	for (int i = 0; i < HASH_8BITS; i++) {
		list_for_each_entry_safe(entry, q, &mac_port_map.hash_table[i], list) {
			list_delete_entry(&entry->list);
			free(entry);
		}
	}
	pthread_mutex_unlock(&mac_port_map.lock);
}

// lookup the mac address in mac_port table
/*
	1. 加锁 (pthread_mutex_lock)。
	2. 使用hash8对mac进行哈希。
	3. 根据哈希值在表中查找对应的iface_info_t*。
	4. 解锁 (pthread_mutex_unlock)。
	5. 返回查找到的接口指针，未找到则返回NULL。
*/
iface_info_t *lookup_port(u8 mac[ETH_ALEN])
{
	// TODO: implement the lookup process here
	pthread_mutex_lock(&mac_port_map.lock);

	int hash_index = hash8((char *)mac, ETH_ALEN);

	mac_port_entry_t *pos = NULL;
	list_for_each_entry(pos, &mac_port_map.hash_table[hash_index], list)
	{
		if (memcmp(pos->mac, mac, ETH_ALEN) == 0)
		{
			pthread_mutex_unlock(&mac_port_map.lock);
			return pos->iface;
		}
	}

	log(DEBUG, "MAC address " ETHER_STRING " not found in mac_port table.", ETHER_FMT(mac));
	pthread_mutex_unlock(&mac_port_map.lock);
	return NULL;
}

// insert the mac -> iface mapping into mac_port table
/*
	1. 加锁。
	2. 使用hash8对mac进行哈希。
	3. 查找mac的哈希值是否已在表中：
	4. 若存在，更新其对应的接口为iface，并刷新其老化时间戳（例如，记录当前时间）。
	5. 若不存在，插入新条目 (mac, iface, current_time)。
	6. 解锁。
*/
void insert_mac_port(u8 mac[ETH_ALEN], iface_info_t *iface)
{
	// TODO: implement the insertion process here
	pthread_mutex_lock(&mac_port_map.lock);

	int hash_index = hash8((char *)mac, ETH_ALEN);

	mac_port_entry_t *pos = NULL;
	list_for_each_entry(pos, &mac_port_map.hash_table[hash_index], list)
	{
		if (memcmp(pos->mac, mac, ETH_ALEN) == 0)
		{
			pos->iface = iface;
			pos->visited = time(NULL);
			pthread_mutex_unlock(&mac_port_map.lock);
			return;
		}
	}

	log(DEBUG, "MAC address " ETHER_STRING " not found in mac_port table, inserting new entry.", ETHER_FMT(mac));
	mac_port_entry_t *new_entry = malloc(sizeof(mac_port_entry_t));
	if (new_entry == NULL) {
		log(ERROR, "Failed to allocate memory for new mac_port_entry.");
		pthread_mutex_unlock(&mac_port_map.lock);
		return;
	}
	memcpy(new_entry->mac, mac, ETH_ALEN);
	new_entry->iface = iface;
	new_entry->visited = time(NULL);
	list_add_tail(&new_entry->list, &mac_port_map.hash_table[hash_index]);

	pthread_mutex_unlock(&mac_port_map.lock);
	return;
}

// dumping mac_port table
void dump_mac_port_table()
{
	mac_port_entry_t *entry = NULL;
	time_t now = time(NULL);

	fprintf(stdout, "dumping the mac_port table:\n");
	pthread_mutex_lock(&mac_port_map.lock);
	for (int i = 0; i < HASH_8BITS; i++) {
		list_for_each_entry(entry, &mac_port_map.hash_table[i], list) {
			fprintf(stdout, ETHER_STRING " -> %s, %d\n", ETHER_FMT(entry->mac), \
					entry->iface->name, (int)(now - entry->visited));
		}
	}

	pthread_mutex_unlock(&mac_port_map.lock);
}

// sweeping mac_port table, remove the entry which has not been visited in the last 30 seconds.
/*
	1. 加锁。
	2. 遍历MAC地址表中的所有条目。
	3. 对于每个条目，检查其老化时间戳。如果 current_time - entry_timestamp > AGING_TIME_SECONDS (例如30秒)，则从表中删除该条目。
	4. 解锁。
	5. 返回本次操作删除的条目数量。
*/
int sweep_aged_mac_port_entry()
{
	// TODO: implement the sweeping process here
	pthread_mutex_lock(&mac_port_map.lock);

	int cnt = 0;

	mac_port_entry_t *pos;
	mac_port_entry_t *q;
	for (int i = 0; i < HASH_8BITS; i++)
	{
		list_for_each_entry_safe(pos, q, &mac_port_map.hash_table[i], list)
		{
			if(time(NULL) - pos->visited > MAC_PORT_TIMEOUT)
			{
				cnt++;

				log(DEBUG, "MAC address " ETHER_STRING " is aged out, removing it.", ETHER_FMT(pos->mac));
				list_delete_entry(&pos->list);
				free(pos);
			}
		}
	}

	pthread_mutex_unlock(&mac_port_map.lock);
	return cnt;
}

// sweeping mac_port table periodically, by calling sweep_aged_mac_port_entry
void *sweeping_mac_port_thread(void *nil)
{
	while (1) {
		sleep(1);
		int n = sweep_aged_mac_port_entry();

		if (n > 0)
			log(DEBUG, "%d aged entries in mac_port table are removed.", n);
	}

	return NULL;
}
