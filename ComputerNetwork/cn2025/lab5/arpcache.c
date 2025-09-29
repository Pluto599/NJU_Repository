#include "arpcache.h"
#include "arp.h"
#include "ether.h"
#include "ip.h"
#include "icmp.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>

static arpcache_t arpcache;

// initialize IP->mac mapping, request list, lock and sweep thread
void arpcache_init()
{
	bzero(&arpcache, sizeof(arpcache_t));

	init_list_head(&(arpcache.req_list));

	pthread_mutex_init(&arpcache.lock, NULL);

	pthread_create(&arpcache.thread, NULL, arpcache_sweep, NULL);
}

// release all the resources when exiting
void arpcache_destroy()
{
	pthread_mutex_lock(&arpcache.lock);

	struct arp_req *req_entry = NULL, *req_q;
	list_for_each_entry_safe(req_entry, req_q, &(arpcache.req_list), list) {
		struct cached_pkt *pkt_entry = NULL, *pkt_q;
		list_for_each_entry_safe(pkt_entry, pkt_q, &(req_entry->cached_packets), list) {
			list_delete_entry(&(pkt_entry->list));
			free(pkt_entry->packet);
			free(pkt_entry);
		}

		list_delete_entry(&(req_entry->list));
		free(req_entry);
	}

	pthread_kill(arpcache.thread, SIGTERM);

	pthread_mutex_unlock(&arpcache.lock);
}

// TODO 3.1 finished
// 查找IP->MAC映射，需要上锁
// 遍历ARP缓存表以查找是否存在与给定IP地址相同的IP->MAC映射条目
// 返回值是true/false，查找成功的MAC地址保存在u8 mac[ETH_ALEN]中
int arpcache_lookup(u32 ip4, u8 mac[ETH_ALEN])
{
	pthread_mutex_lock(&arpcache.lock);

	for (int i = 0; i < MAX_ARP_SIZE; i++) 
	{
		if (arpcache.entries[i].valid && arpcache.entries[i].ip4 == ip4) 
		{
			memcpy(mac, arpcache.entries[i].mac, ETH_ALEN);
			pthread_mutex_unlock(&arpcache.lock);
			return 1;
		}
	}

	pthread_mutex_unlock(&arpcache.lock);
	return 0;
}

// TODO 3.2 finished
// 将IP->MAC映射插入到arpcache中，需要上锁
// 如果ARP缓存表中存在超时条目(valid = 0)，就替换它
// 如果ARP缓存表中不存在超时条目，就随机替换一个
// 如果有等待此IP->MAC映射的待发送数据包，则为每个数据包填充以太网头，并将其发送出去
// Tips: arpcache_t是完整的arp缓存表，里边的req_list是一个链表，它的每个节点(用arp_req结构体封装)里又存着一个链表头，这些二级链表(节点类型是cached_pkt)缓存着相同目标ip但不知道mac地址的包
void arpcache_insert(u32 ip4, u8 mac[ETH_ALEN])
{
	pthread_mutex_lock(&arpcache.lock);

	int i;
	// choose index i
	for (i = 0; i < MAX_ARP_SIZE; i++)
	{
		if(!arpcache.entries[i].valid) 
			break;
	}
	if (i == MAX_ARP_SIZE)
	{
		i = rand() % MAX_ARP_SIZE;
	}
	// replace the entry
	arpcache.entries[i].ip4 = ip4;
	memcpy(arpcache.entries[i].mac, mac, ETH_ALEN);
	arpcache.entries[i].added = time(NULL);
	arpcache.entries[i].valid = 1;

	// handle pending packets
	struct arp_req *pos = NULL;
	struct arp_req *q = NULL;
	list_for_each_entry_safe(pos,q, &(arpcache.req_list), list)
	{
		if (pos->ip4 == ip4)
		{
			struct cached_pkt *pkt_pos = NULL;
			struct cached_pkt *pkt_q = NULL;
			list_for_each_entry_safe(pkt_pos, pkt_q, &(pos->cached_packets), list)
			{
				char *packet = pkt_pos->packet;
				int len = pkt_pos->len;
				struct ether_header *eth_hdr = (struct ether_header *)packet;
				memcpy(eth_hdr->ether_dhost, mac, ETH_ALEN);

				log(DEBUG, "send pending packet, src_ip: " IP_FMT ", dst_ip: " IP_FMT, HOST_IP_FMT_STR(pos->iface->ip), HOST_IP_FMT_STR(pos->ip4));
				iface_send_packet(pos->iface, packet, len);

				list_delete_entry(&(pkt_pos->list));
				free(pkt_pos);
			}
			list_delete_entry(&(pos->list));
			free(pos);

			break;
		}
	}

	pthread_mutex_unlock(&arpcache.lock);
	return;
}

// TODO 3.3 finished
// 在存储待处理数据包的链表中查找，如果已存在需要相同IP->MAC映射的条目，
// 这说明已经发送过有关ARP Request，只需将该数据包附加到该条目链表的末尾（该条目可能包含多个数据包）
// 否则，malloc一个新条目，在其链表上附加数据包，并发送ARP Request
// Tips:
// arpcache_t是完整的arp缓存表，里边的req_list是一个链表，它的每个节点(类型是arp_req)里又存着一个链表头，这些二级链表(节点类型是cached_pkt)缓存着相同目标ip但不知道mac地址的包
void arpcache_append_packet(iface_info_t *iface, u32 ip4, char *packet, int len)
{
	pthread_mutex_lock(&arpcache.lock);

	// init cached_pkt
	struct cached_pkt *cache_pkt= (struct cached_pkt *)malloc(sizeof(struct cached_pkt));
	if (!cache_pkt)
	{
		log(ERROR, "malloc failed when appending packet");
		pthread_mutex_unlock(&arpcache.lock);
		return;
	}

	cache_pkt->packet = packet;
	cache_pkt->len = len;

	struct arp_req *pos = NULL, *q = NULL;
	list_for_each_entry_safe(pos, q, &arpcache.req_list, list)
	{
		// find same ip4
		if (pos->ip4 == ip4)
		{
			log(DEBUG, "append packet to existing arp request, ip: " IP_FMT, HOST_IP_FMT_STR(ip4));

			list_add_tail(&(cache_pkt->list), &(pos->cached_packets));

			pthread_mutex_unlock(&arpcache.lock);
			return;
		}
	}

	// not find, create a new entry and send arp request
	log(DEBUG," create new arp request, ip: " IP_FMT, HOST_IP_FMT_STR(ip4));

	struct arp_req *new_req = (struct arp_req *)malloc(sizeof(struct arp_req));
	if (!new_req)
	{
		log(ERROR, "malloc failed when appending packet");
		pthread_mutex_unlock(&arpcache.lock);
		return;
	}

	new_req->iface = iface;
	new_req->ip4 = ip4;
	init_list_head(&(new_req->cached_packets));
	list_add_tail(&(cache_pkt->list), &(new_req->cached_packets));
	list_add_tail(&(new_req->list), &(arpcache.req_list));
	
	pthread_mutex_unlock(&arpcache.lock);
	
	// send arp request
	log(DEBUG, "send arp request, src_ip: " IP_FMT ", dst_ip: " IP_FMT, HOST_IP_FMT_STR(iface->ip), HOST_IP_FMT_STR(ip4));
	arp_send_request(iface, ip4);
	new_req->sent = time(NULL);
	new_req->retries = 1;
	return;
}

// TODO 3.4 finished
// 每一秒扫描一次arpcache
// 对于每个IP->MAC映射，如果该条目在ARP缓存表中已存在超过15秒，则将其valid属性置为0
// 对于正在进行ARP Request的条目，如果1秒前发出过一次请求，但仍未收到答复，则重传并将重传计数+1
// 如果重传次数已达五次而未收到ARP Reply，则对每个待处理数据包的源IP地址，发送ICMP Error Packet (DEST_HOST_UNREACHABLE)，并丢弃这些数据包
// 注意，在arpcache_append_packet第一次发送ARP Request时，就算做一次重传，
// 所以这里与其理解为重传，不如理解为发送ARP Request的次数
// tips
// arpcache_t是完整的arp缓存表，里边的req_list是一个链表，它的每个节点(类型是arp_req)里又存着一个链表头，这些二级链表(节点类型是cached_pkt)缓存着相同目标ip但不知道mac地址的包
void *arpcache_sweep(void *arg) 
{
	while (1) {
		sleep(0.5);

		pthread_mutex_lock(&arpcache.lock);

		for (int i = 0; i < MAX_ARP_SIZE; i++)
		{
			if (!arpcache.entries[i].valid)
				continue;

			if (time(NULL) - arpcache.entries[i].added > ARP_ENTRY_TIMEOUT)
			{
				log(DEBUG, "arp cache timeout, set invalid, ip: " IP_FMT, HOST_IP_FMT_STR(arpcache.entries[i].ip4));
				arpcache.entries[i].valid = 0;
			}
		}

		struct arp_req *pos = NULL, *q = NULL;
		list_for_each_entry_safe(pos, q, &arpcache.req_list, list)
		{
			if (time(NULL) - pos->sent >= 1)
			{
				log(DEBUG, "arp request retry, ip: " IP_FMT ", retries: %d", HOST_IP_FMT_STR(pos->ip4), pos->retries);

				if (pos->retries < ARP_REQUEST_MAX_RETRIES)
				{
					log(DEBUG, "send arp request again, src_ip: " IP_FMT ", dst_ip: " IP_FMT, HOST_IP_FMT_STR(pos->iface->ip), HOST_IP_FMT_STR(pos->ip4));

					pos->sent = time(NULL);
					pos->retries++;
					arp_send_request(pos->iface, pos->ip4);
				}
				else
				{
					log(DEBUG, "arp request failed, ip: " IP_FMT , HOST_IP_FMT_STR(pos->ip4), pos->retries);

					struct cached_pkt *pkt_pos = NULL, *pkt_q = NULL;
					list_for_each_entry_safe(pkt_pos, pkt_q, &(pos->cached_packets), list)
					{
						char *packet = pkt_pos->packet;
						int len = pkt_pos->len;
						struct iphdr *ip_hdr = packet_to_ip_hdr(packet);

						log(DEBUG, "retry 5 times without reply, send icmp error packet");
						
						pthread_mutex_unlock(&arpcache.lock);
						icmp_send_packet(packet, len, ICMP_DEST_UNREACH, ICMP_HOST_UNREACH);
						pthread_mutex_lock(&arpcache.lock);

						list_delete_entry(&(pkt_pos->list));
						free(pkt_pos);
					}
					list_delete_entry(&(pos->list));
					free(pos);
				}
			}
		}
		pthread_mutex_unlock(&arpcache.lock);
	}

	return NULL;
}
