#include "ip.h"
#include "icmp.h"
#include "arpcache.h"
#include "rtable.h"
#include "arp.h"
#include "log.h"
#include <stdlib.h>
#include <assert.h>

// initialize ip header 
void ip_init_hdr(struct iphdr *ip, u32 saddr, u32 daddr, u16 len, u8 proto)
{
	ip->version = 4;
	ip->ihl = 5;
	ip->tos = 0;
	ip->tot_len = htons(len);
	ip->id = rand();
	ip->frag_off = htons(IP_DF);
	ip->ttl = DEFAULT_TTL;
	ip->protocol = proto;
	ip->saddr = htonl(saddr);
	ip->daddr = htonl(daddr);
	ip->checksum = ip_checksum(ip);
}

// TODO 1 finished
// 在路由表中查找，找到具有相同且最长前缀的条目
// 输入IP地址为主机序，应该在handle_ip_packet就转换成主机序了
// 路由表是全局变量rtable，定义在rtable.c中
rt_entry_t *longest_prefix_match(u32 dst)
{
	int max_prefix_len = -1;
	rt_entry_t *best_match = NULL;
	rt_entry_t *pos = NULL;

	list_for_each_entry(pos, &rtable, list)
	{
		if ((dst & pos->mask) == (pos->dest & pos->mask))
		{
			int prefix_len = 0;
			u32 mask = pos->mask;
			while (mask)
			{
				prefix_len += (mask & 1);
				mask >>= 1;
			}
			if (prefix_len > max_prefix_len)
			{
				max_prefix_len = prefix_len;
				best_match = pos;
			}
		}
	}
	return best_match;
}

// TODO 6 finished
// 与ip_forward_packet函数不同，ip_send_packet发送的是由路由器自身产生的数据包
// 本实验中该函数仅用于发送ICMP数据包，但总体实现与ip_forward_packet类似
// 你可以在完成icmp_send_packet之后再完成该函数，也可以直接将该函数的功能集成到icmp_send_packet中
// 发送数据包的接口由longest_prefix_match指定
// 如果icmp_send_packet中更新过checksum和TTL了就不需要在该函数中更新了
// 1.处理发送给路由器自身的 icmp 数据包（ICMP ECHO REPLY）
// 2.当发生错误时，发送 icmp 错误数据包。
// 注意，这两个icmp数据包的结构不同，需要malloc不同大小的内存。
// 以及，包中有些字段可能在前几个函数中已经被转换成主机序了，但不要忘记仍有部分字段需要转换
// 注意更新checksum的时机，以及可以利用ip_base.c中定义的ip_init_hdr来初始化ip头部
// ip.h/icmp.h中定义了一些函数和宏，说不定有用
void ip_send_packet(char *packet, int len)
{
	struct iphdr *ip_hdr = packet_to_ip_hdr(packet);
	u32 src_ip = ntohl(ip_hdr->saddr);
	u32 dst_ip = ntohl(ip_hdr->daddr);

	rt_entry_t *entry = longest_prefix_match(dst_ip);
	if (!entry)
	{
		log(ERROR, "ip_send_packet: no route to destination " IP_FMT, HOST_IP_FMT_STR(dst_ip));

		free(packet);
		return;
	}

	u32 nxt_hop = (entry->gw == 0) ? dst_ip : entry->gw;

	log(DEBUG, "ip_send_packet: sending packet with src_ip: " IP_FMT " , dst_ip: " IP_FMT " , via %s", HOST_IP_FMT_STR(src_ip), HOST_IP_FMT_STR(dst_ip), entry->iface->name);

	iface_send_packet_by_arp(entry->iface, nxt_hop, packet, len);

	return;
}
