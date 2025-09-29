
#include "arp.h"
#include "base.h"
#include "types.h"
#include "ether.h"
#include "arpcache.h"
#include "log.h"
#include "ip.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// TODO 2.1 finished
// 如果ARP数据包的目标IP地址与收到它的接口的IP地址不相等，则丢弃它
// 否则，如果是ARP Request数据包，则发送ARP Reply数据包，并插入该数据包的IP->MAC映射
//      如果是ARP Reply数据包，则插入该数据包的IP->MAC映射
// Tips:
// You can use functions: htons, htonl, ntohs, ntohl to convert host byte order and network byte order (16 bits use ntohs/htons, 32 bits use ntohl/htonl).
// You can use function: packet_to_ether_arp() in arp.h to get the ethernet header in a packet.
void handle_arp_packet(iface_info_t *iface, char *packet, int len)
{
	struct ether_arp *hdr = packet_to_ether_arp(packet);
	u32 src_ip = ntohl(hdr->arp_spa);
	u32 dst_ip = ntohl(hdr->arp_tpa);

	if (dst_ip != iface->ip)
	{
		log(DEBUG, "dropped arp packet because dst not equals to iface ip");
		log(ERROR, "ARP packet dropped, dst_ip: " IP_FMT ", iface_ip: " IP_FMT, HOST_IP_FMT_STR(dst_ip), HOST_IP_FMT_STR(iface->ip));
		free(packet);
		return;
	}

	u16 op = ntohs(hdr->arp_op);
	log(DEBUG, "Received ARP packet, op: %s, src_ip: " IP_FMT ", dst_ip: " IP_FMT,
		(op == ARPOP_REQUEST) ? "REQUEST" : (op == ARPOP_REPLY) ? "REPLY" : "UNKNOWN",
		HOST_IP_FMT_STR(src_ip), HOST_IP_FMT_STR(dst_ip));

	if(op == ARPOP_REQUEST)
	{
		log(DEBUG, "ARP request received, src_ip: " IP_FMT ", dst_ip: " IP_FMT, HOST_IP_FMT_STR(src_ip), HOST_IP_FMT_STR(dst_ip));
		arp_send_reply(iface, hdr);
		arpcache_insert(src_ip, hdr->arp_sha);
	}
	else if(op == ARPOP_REPLY)
	{
		log(DEBUG, "ARP reply received, src_ip: " IP_FMT ", dst_ip: " IP_FMT, HOST_IP_FMT_STR(src_ip), HOST_IP_FMT_STR(dst_ip));
		arpcache_insert(src_ip, hdr->arp_sha);
	}
	else
	{
		log(ERROR,"Unknown ARP packet type");
		free(packet);
		return;
	}
}

// TODO 2.2 finished
// 封装ARP Reply数据包，并发送出去
void arp_send_reply(iface_info_t *iface, struct ether_arp *req_hdr)
{
	int len = ETHER_HDR_SIZE + sizeof(struct ether_arp);
	char *packet = (char *)malloc(len);
	if(!packet)
	{
		log(ERROR, "malloc failed when sending arp reply");
		return;
	}
	
	// fill ether header
	struct ether_header *eth_hdr = (struct ether_header *)packet;
	memcpy(eth_hdr->ether_dhost, req_hdr->arp_sha, ETH_ALEN);
	memcpy(eth_hdr->ether_shost, iface->mac, ETH_ALEN);
	eth_hdr->ether_type = htons(ETH_P_ARP);

	// fill arp
	struct ether_arp *arp = (struct ether_arp *)(packet + ETHER_HDR_SIZE);
	arp->arp_hrd = htons(ARPHRD_ETHER);
	arp->arp_pro = htons(ETH_P_IP);
	arp->arp_hln = ETH_ALEN;
	arp->arp_pln = 4;
	arp->arp_op = htons(ARPOP_REPLY);
	memcpy(arp->arp_sha, iface->mac, ETH_ALEN);
	arp->arp_spa = htonl(iface->ip);
	memcpy(arp->arp_tha, req_hdr->arp_sha, ETH_ALEN);
	arp->arp_tpa = req_hdr->arp_spa;

	u32 spa = ntohl(arp->arp_spa);
	u32 tpa = ntohl(arp->arp_tpa);
	log(DEBUG, "ARP reply sent, src_ip: " IP_FMT ", dst_ip: " IP_FMT,HOST_IP_FMT_STR(spa), HOST_IP_FMT_STR(tpa));

	iface_send_packet(iface, packet, len);
	return;
}

// TODO 2.3 finished
// 封装ARP Request数据包，并发送出去
void arp_send_request(iface_info_t *iface, u32 dst_ip)
{
	int len = ETHER_HDR_SIZE + sizeof(struct ether_arp);
	char *packet = (char *)malloc(len);
	if (!packet)
	{
		log(ERROR, "malloc failed when sending arp request");
		return;
	}

	// fill ether header
	struct ether_header *eth_hdr = (struct ether_header *)packet;
	memset(eth_hdr->ether_dhost, 0xff, ETH_ALEN);
	memcpy(eth_hdr->ether_shost, iface->mac, ETH_ALEN);
	eth_hdr->ether_type = htons(ETH_P_ARP);

	// fill arp
	struct ether_arp *arp = (struct ether_arp *)(packet + ETHER_HDR_SIZE);
	arp->arp_hrd = htons(ARPHRD_ETHER);
	arp->arp_pro = htons(ETH_P_IP);
	arp->arp_hln = ETH_ALEN;
	arp->arp_pln = 4;
	arp->arp_op = htons(ARPOP_REQUEST);
	memcpy(arp->arp_sha, iface->mac, ETH_ALEN);
	arp->arp_spa = htonl(iface->ip);
	memset(arp->arp_tha, 0, ETH_ALEN);
	arp->arp_tpa = htonl(dst_ip);

	u32 spa = ntohl(arp->arp_spa);
	u32 tpa = ntohl(arp->arp_tpa);
	log(DEBUG, "ARP request sent, src_ip: " IP_FMT ", dst_ip: " IP_FMT,HOST_IP_FMT_STR(spa), HOST_IP_FMT_STR(tpa));

	iface_send_packet(iface, packet, len);
	return;
}

// TODO 7
// 在ARP缓存表中查找目标IP的MAC地址
// 如果找到，则填充以太网帧头部并通过iface_send_packet函数发送数据包
// 否则，将此数据包挂入arpcache并发送ARP Request
void iface_send_packet_by_arp(iface_info_t *iface, u32 dst_ip, char *packet, int len)
{
	struct ether_header *eth_hdr = packet_to_ether_hdr(packet);
	u8 src_mac[ETH_ALEN], dst_mac[ETH_ALEN];

	// set src mac
	memcpy(src_mac, iface->mac, ETH_ALEN);

	// set dst mac
	if (arpcache_lookup(dst_ip, dst_mac))
	{
		// found in ARP cache
		memcpy(eth_hdr->ether_dhost, dst_mac, ETH_ALEN);

		log(DEBUG, "ARP cache hit, sending packet to " IP_FMT, HOST_IP_FMT_STR(dst_ip));

		iface_send_packet(iface, packet, len);
	}
	else
	{
		// not found, append packet to arpcache
		log(DEBUG, "ARP cache miss, appending packet to arpcache and sending ARP request");
		arpcache_append_packet(iface, dst_ip, packet, len);
	}
}
