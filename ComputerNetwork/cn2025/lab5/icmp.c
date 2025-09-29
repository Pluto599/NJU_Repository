#include "icmp.h"
#include "ip.h"
#include "rtable.h"
#include "arp.h"
#include "base.h"
#include "log.h"
#include <stdlib.h>
#include <assert.h>

/*
1. 路由表查找失败
Type：3
Code：0
Checksum：使用icmp.h文件中的icmp_checksum进行更新，注意更新Checksum的时机
Rest of ICMP Header： 还剩4字节，均设置为0
主体部分：拷贝发生错误的数据包的IP头部（>=20字节）+ 随后的8字节

2. ARP Request失败
Type：3
Code：1
Checksum：同上
Rest of ICMP Header：同上

3. TTL值减为0
Type：11
Code：0
Checksum：同上
Rest of ICMP Header：同上

4. 收到Ping本端口的数据包（ ICMP EchoRequest Packet，头部Type为8 ）
Type：0
Code：0
Checksum：同上
Rest of ICMP Header以及主体内容：拷贝Ping包中的相应字段
*/

// TODO 5 finished
// 1.处理发送给路由器自身的 icmp 数据包（ICMP ECHO REPLY）
// 2.当发生错误时，发送 icmp 错误数据包。
// 注意，这两个icmp数据包的结构不同，需要malloc不同大小的内存。
// 以及，包中有些字段可能在前几个函数中已经被转换成主机序了，但不要忘记仍有部分字段需要转换
// 注意更新checksum的时机，以及可以利用ip_base.c中定义的ip_init_hdr来初始化ip头部
// ip.h/icmp.h中定义了一些函数和宏，说不定有用
void icmp_send_packet(const char *in_pkt, int len, u8 type, u8 code)
{
	log(DEBUG, "icmp_send_packet: type: %d, code: %d", type, code);

	// icmp echo reply packet
	if(type == ICMP_ECHOREPLY)
	{
		char* packet=(char *)malloc(len);
		if (!packet)
		{
			log(ERROR, "malloc failed when sending icmp echo reply packet");
			return;
		}
		memcpy(packet, in_pkt, len);

		struct ether_header *eth_hdr = packet_to_ether_hdr(packet);
		u8 tmp_mac[ETH_ALEN];
		memcpy(tmp_mac, eth_hdr->ether_dhost, ETH_ALEN);
		memcpy(eth_hdr->ether_dhost, eth_hdr->ether_shost, ETH_ALEN);
		memcpy(eth_hdr->ether_shost, tmp_mac, ETH_ALEN);

		struct iphdr *ip_hdr = packet_to_ip_hdr(packet);
		u32 tmp_ip = ip_hdr->daddr;
		ip_hdr->daddr = ip_hdr->saddr;
		ip_hdr->saddr = tmp_ip;
		ip_init_hdr(ip_hdr, ntohl(ip_hdr->saddr), ntohl(ip_hdr->daddr), len - ETHER_HDR_SIZE, IPPROTO_ICMP);

		// update icmp checksum
		struct icmphdr *icmp_hdr = packet_to_icmp_hdr(packet);
		icmp_hdr->type = ICMP_ECHOREPLY;
		int icmp_len = len - ETHER_HDR_SIZE - IP_HDR_SIZE(packet_to_ip_hdr(packet));
		icmp_hdr->checksum = icmp_checksum(icmp_hdr, icmp_len);

		log(DEBUG, "icmp_send_packet: send icmp echo reply packet");
		ip_send_packet(packet, len);

		free(in_pkt);
		return;
	}
	// icmp error packet
	else
	{
		char *packet = (char *)malloc(ETHER_HDR_SIZE + IP_BASE_HDR_SIZE + ICMP_HDR_SIZE + IP_HDR_SIZE(packet_to_ip_hdr(in_pkt)) + 8);
		if (!packet)
		{
			log(ERROR, "malloc failed when sending icmp error packet");
			free(in_pkt);
			return;
		}

		struct ether_header *in_eth_hdr = packet_to_ether_hdr(in_pkt);
		struct iphdr *in_ip_hdr = packet_to_ip_hdr(in_pkt);

		// set ether header
		struct ether_header *eth_hdr = (struct ether_header *)packet;
		memset(eth_hdr->ether_dhost, 0, ETH_ALEN); // set in iface_send_packet_by_arp
		memset(eth_hdr->ether_shost, 0, ETH_ALEN); // set in iface_send_packet_by_arp
		eth_hdr->ether_type = htons(ETH_P_IP);

		// set icmp header
		struct icmphdr *icmp_hdr = (struct icmphdr *)(packet + ETHER_HDR_SIZE + IP_BASE_HDR_SIZE);
		icmp_hdr->type = type;
		icmp_hdr->code = code;
		icmp_hdr->icmp_identifier = htons(0);
		icmp_hdr->icmp_sequence = htons(0);
		icmp_hdr->checksum = 0;

		// set rest of icmp header
		memcpy(packet + ETHER_HDR_SIZE + IP_BASE_HDR_SIZE + ICMP_HDR_SIZE, in_ip_hdr, IP_HDR_SIZE(in_ip_hdr) + 8);

		// update icmp checksum
		int icmp_len = ICMP_HDR_SIZE + IP_HDR_SIZE(in_ip_hdr) + 8;
		icmp_hdr->checksum = icmp_checksum(icmp_hdr, icmp_len);
		
		// set ip header
		struct iphdr *ip_hdr = (struct iphdr *)(packet + ETHER_HDR_SIZE);
		u32 dst_ip = ntohl(in_ip_hdr->saddr);
		rt_entry_t *entry = longest_prefix_match(dst_ip);
		u32 src_ip = entry->iface->ip;
		u16 ip_len = IP_BASE_HDR_SIZE + ICMP_HDR_SIZE + IP_HDR_SIZE(in_ip_hdr) + 8;
		ip_init_hdr(ip_hdr, src_ip, dst_ip, ip_len, IPPROTO_ICMP);
		
		log(DEBUG, "icmp_send_packet: send icmp error packet, type: %d, code: %d", type, code);
		ip_send_packet(packet, ETHER_HDR_SIZE + IP_BASE_HDR_SIZE + ICMP_HDR_SIZE + IP_HDR_SIZE(in_ip_hdr) + 8);

		free(in_pkt);
		return;
	}
}
