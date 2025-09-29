#ifndef __RTABLE_H__
#define __RTABLE_H__

#include "base.h"
#include "types.h"

#include "list.h"

// structure of ip forwarding table
// note: 1, the table supports only ipv4 address;
// 		 2, addresses are stored in host byte order.
typedef struct {
	struct list_head list;	// 链表实现
	u32 dest;				// destination ip address (could be network or host) 目的IP地址
	u32 mask;				// network mask of dest 网络掩码
	u32 gw;					// ip address of next hop (will be 0 if dest is in the same network with iface) 下一跳网关地址（如果目的IP和路由器端口在同一个子网中就是0）
	int flags;				// flags (could be omitted here) 转发表条目标识（可忽略）
	char if_name[16];		// name of the interface 转出端口名字, e.g. r1-eth0
	iface_info_t *iface;	// pointer to the interface structure 转出端口
} rt_entry_t; // 路由表条目

extern struct list_head rtable; // 路由表，是一个链表头部伪节点

void init_rtable();
void load_static_rtable();
void clear_rtable();
void add_rt_entry(rt_entry_t *entry);
void remove_rt_entry(rt_entry_t *entry);
void print_rtable();
rt_entry_t *new_rt_entry(u32 dest, u32 mask, u32 gw, iface_info_t *iface);

rt_entry_t *longest_prefix_match(u32 ip);

void read_kernel_rtable(struct list_head *rtable);
void load_rtable_from_kernel();
void load_rtable(struct list_head *rtable);

#endif
