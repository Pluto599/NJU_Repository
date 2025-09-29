#ifndef __MAC_H__
#define __MAC_H__

#include "base.h"
#include "hash.h"
#include "list.h"

#include <pthread.h>
#include <unistd.h>

#define MAC_PORT_TIMEOUT 30

struct mac_port_entry
{
	struct list_head list; // 链表节点，用于将条目链接到哈希表的链表中
	uint8_t mac[ETH_ALEN]; // MAC地址（6字节），作为查找的键值
	iface_info_t *iface;   // 指向接口信息的指针，表示该MAC地址对应的出端口
	time_t visited;		   // 时间戳，记录该条目最后一次被访问的时间，用于老化机制
};

typedef struct mac_port_entry mac_port_entry_t;

typedef struct {
	struct list_head hash_table[HASH_8BITS];
	pthread_mutex_t lock;
	pthread_t thread;
} mac_port_map_t;

void *sweeping_mac_port_thread(void *);
void init_mac_port_table();
void destory_mac_port_table();
void dump_mac_port_table();
iface_info_t *lookup_port(uint8_t mac[ETH_ALEN]);
void insert_mac_port(uint8_t mac[ETH_ALEN], iface_info_t *iface);
int sweep_aged_mac_port_entry();

#endif
