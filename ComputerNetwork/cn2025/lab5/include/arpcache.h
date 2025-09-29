#ifndef __ARPCACHE_H__
#define __ARPCACHE_H__

#include "base.h"
#include "types.h"
#include "list.h"

#include <pthread.h>

#define MAX_ARP_SIZE 32			  // maximum number of IP->mac mapping entries
#define ARP_ENTRY_TIMEOUT 15	  // timeout for IP->mac entry
#define ARP_REQUEST_MAX_RETRIES 5 // maximum number of retries of arp request

// pending packet, waiting for arp reply
// 第二层链表节点，每个cached_pkt代表一个不知道MAC地址的被缓存了的packet
struct cached_pkt
{
	struct list_head list; // 第二层链表节点
	char *packet;		   // packet
	int len;			   // the length of packet
};

// list of pending packets, with the same iface and destination ip address
// 第一层链表节点，每个arp_req代表着一个packet集合，它们的目标IP相同，但不知道MAC地址
struct arp_req
{
	struct list_head list; // 第一层链表节点
	iface_info_t *iface; // the interface that will send the pending packets 转发数据包的端口
	u32 ip4; // destination ip address 等待ARP Reply的IP地址
	time_t sent; // last time when arp request is sent ARP发送时间
	int retries; // number of retries 重试次数
	struct list_head cached_packets; // pending packets 链表头，缓存着相同目标IP但不知道MAC地址的packet集合，指向cached_pkt
};

// IP->MAC映射
struct arp_cache_entry
{
	u32 ip4; // destination ip address, stored in host byte order IP地址，主机序
	u8 mac[ETH_ALEN]; // mac address IP地址对应的MAC地址
	time_t added; // the time when this entry is inserted 添加时间
	int valid; // whether this entry is valid (has not triggered the timeout) 是否仍然有效，若超时应设置为0
};

// ARP缓存表
typedef struct
{
	struct arp_cache_entry entries[MAX_ARP_SIZE]; // IP->max mapping entries IP->MAC映射，最多存储32条
	struct list_head req_list; // the pending packet list 链表头，存着等待ARP Reply的IP列表，指向arp_req
	pthread_mutex_t lock; // each operation on arp cache should apply the lock first ARP缓存表查询、更新时需要的锁
	pthread_t thread; // the id of the arp cache sweeping thread 清理ARP缓存表用的的线程
} arpcache_t;

void arpcache_init();
void arpcache_destroy();

int arpcache_lookup(u32 ip4, u8 mac[]);
void arpcache_insert(u32 ip4, u8 mac[]);
void arpcache_append_packet(iface_info_t *iface, u32 ip4, char *packet, int len);
void *arpcache_sweep(void *arg);

// TODO:ARP缓存操作
/*
1. 查找IP->MAC映射
	a. 如果在ARP缓存表中找到有关映射，就填充数据包的目的MAC地址，并转发该数据包。
	b. 否则，将该数据包缓存在ARP缓存表的等待队列中（用链表实现），并发送ARP Request。
2. 收到新的IP->MAC映射
	a. 将该映射写入ARP缓存表中 ，如果ARP缓存表已满，则随机替换掉其中一个。
	b. 将ARP缓存表等待队列中等待该映射的数据包依次填入目的MAC地址并转发出去，再删掉相应缓存。
3. 同时每秒对ARP缓存表进行清理
	a. 如果一个缓存条目在缓存中已存在超过了15秒，则将该条目清除。
	b. 如果一个IP对应的ARP Request已经发送超过1秒，则重新发送ARP Request。
	c. 如果ARP Request发送超过5次仍未收到ARP Reply，则对等待该映射的数据包的源IP地址依次发送ICMP Error Packet（Destination Host Unreachable），并删除等待的数据包。
*/

#endif