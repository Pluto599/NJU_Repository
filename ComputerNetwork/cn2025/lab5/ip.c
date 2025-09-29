#include "ip.h"
#include "icmp.h"
#include "arpcache.h"
#include "rtable.h"
#include "arp.h"
#include "log.h"
#include <stdlib.h>
#include <assert.h>

// TODO 4.1 finished
// 如果是ICMP EchoRequest数据包，并且目标IP地址等于收到它的接口的 IP 地址，则发送ICMP EchoReply数据包
// 否则，转发数据包
// Tips:
// You can use struct iphdr *ip = packet_to_ip_hdr(packet); in ip.h to get the ip header in a packet.
// You can use struct icmphdr *icmp = (struct icmphdr *)IP_DATA(ip); in ip.h to get the icmp header in a packet.
void handle_ip_packet(iface_info_t *iface, char *packet, int len)
{
	struct iphdr *ip_hdr = packet_to_ip_hdr(packet);
	u32 src_ip = ntohl(ip_hdr->saddr);
	u32 dst_ip = ntohl(ip_hdr->daddr);
	
	if(ip_hdr->protocol == IPPROTO_ICMP)
	{
		struct icmphdr *icmp_hdr = packet_to_icmp_hdr(packet);

		if(icmp_hdr->type == ICMP_ECHOREQUEST && dst_ip == iface->ip)
		{
			log(DEBUG, "ICMP EchoRequest received, src_ip: " IP_FMT ", dst_ip: " IP_FMT, HOST_IP_FMT_STR(src_ip), HOST_IP_FMT_STR(dst_ip));

			icmp_send_packet(packet, len, ICMP_ECHOREPLY, 0);

			return;
		}
	}

	// otherwise, forward the packet
	log(DEBUG, "other IP packet received, src_ip: " IP_FMT ", dst_ip: " IP_FMT, HOST_IP_FMT_STR(src_ip), HOST_IP_FMT_STR(dst_ip));

	ip_forward_packet(dst_ip, packet, len);
	return;
}

// TODO 4.2 finished
// 转发数据包时，需要检查TTL，更新checksum和TTL
// 然后通过longest_prefix_match确定转发数据包的下一跳和接口，无法确定则要发送ICMP Error Packet
// 最后，通过iface_send_packet_by_arp发送数据包
// 请仔细思考checksum的更新时机，以及需要通过路由表表项属性entry->gw判断下一跳是目标host还是下一个子网
void ip_forward_packet(u32 ip_dst, char *packet, int len)
{
	struct iphdr *ip_hdr = packet_to_ip_hdr(packet);
	ip_hdr->ttl--;

	if (ip_hdr->ttl <= 0)
	{
		log(ERROR, "TTL expired");
		icmp_send_packet(packet, len, ICMP_TIME_EXCEEDED, ICMP_EXC_TTL);

		return;
	}
	else
	{
		// update ip checksum
		ip_hdr->checksum = ip_checksum(ip_hdr);

		rt_entry_t* entry = longest_prefix_match(ip_dst);
		if(entry == NULL)
		{
			log(ERROR, "No route to host");
			icmp_send_packet(packet, len, ICMP_DEST_UNREACH, ICMP_NET_UNREACH);

			return;
		}

		u32 nxt_hop = (entry->gw == 0) ? ip_dst : entry->gw;

		log(DEBUG, "IP packet forwarded, src_ip: " IP_FMT ", dst_ip: " IP_FMT, NET_IP_FMT_STR(ip_hdr->saddr), NET_IP_FMT_STR(ip_hdr->daddr));

		iface_send_packet_by_arp(entry->iface, nxt_hop, packet, len);
		return;
	}
}