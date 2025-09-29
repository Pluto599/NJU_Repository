#include "base.h"
#include "ether.h"
#include "mac.h"
#include "utils.h"

#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// handle packet
// 1. if the dest mac address is found in mac_port table, forward it; otherwise, broadcast it.
// 2. put the src mac -> iface mapping into mac hash table.
/*
1. 从以太网帧头部（packet）中提取源MAC地址和目的MAC地址。

	struct ether_header *eh = (struct ether_header *)packet;
	u8 *src_mac = eh->ether_shost;
	u8 *dst_mac = eh->ether_dhost;

2. 调用 insert_mac_port(src_mac, rx_iface) 来学习/更新源MAC地址信息。
3. 判断目的MAC地址 dst_mac 是否为广播地址 (FF:FF:FF:FF:FF:FF)。

	如果是广播地址，调用 broadcast_packet(rx_iface, packet, len)。
	如果不是广播地址（单播）：
		调用 iface_info_t *tx_iface = lookup_port(dst_mac);。
		如果 tx_iface 非空且 tx_iface != rx_iface，则调用 iface_send_packet(tx_iface, packet, len)。
		如果 tx_iface 为空 (未知单播) 或 tx_iface == rx_iface (目标在同一端口，通常不转发)，则调用 broadcast_packet(rx_iface, packet, len)。
*/
void handle_packet(iface_info_t *iface, char *packet, int len)
{
	// TODO: implement the packet forwarding process here

	struct ether_header *eh = (struct ether_header *)packet;
	u8 *src_mac = eh->ether_shost;
	u8 *dst_mac = eh->ether_dhost;
	// log(DEBUG, "received packet from interface %s, src: " ETHER_STRING ", dst: " ETHER_STRING, iface->name, ETHER_FMT(src_mac), ETHER_FMT(dst_mac));

	insert_mac_port(src_mac, iface);

	if(memcmp(dst_mac, "\xff\xff\xff\xff\xff\xff", ETH_ALEN) == 0) 
	{
		// log(DEBUG, "received broadcast packet, forwarding it to all interfaces.");
		broadcast_packet(iface, packet, len);
		return;
	}
	else
	{
		iface_info_t *tx_iface = lookup_port(dst_mac);
		if(tx_iface && tx_iface != iface)
		{
			// log(DEBUG, "forwarding packet to interface %s.", tx_iface->name);
			iface_send_packet(tx_iface, packet, len);
			return;
		}
		else
		{
			// log(DEBUG, "unknown unicast packet or packet destined to the same interface, broadcasting it.");
			broadcast_packet(iface, packet, len);
			return;
		}
	}

	free(packet);
}

// run user stack, receive packet on each interface, and handle those packet
// like normal switch
void ustack_run()
{
	struct sockaddr_ll addr;
	socklen_t addr_len = sizeof(addr);
	char buf[ETH_FRAME_LEN];
	int len;

	while (1) {
		int ready = poll(instance->fds, instance->nifs, -1);
		if (ready < 0) {
			perror("Poll failed!");
			break;
		}
		else if (ready == 0)
			continue;

		for (int i = 0; i < instance->nifs; i++) {
			if (instance->fds[i].revents & POLLIN) {
				len = recvfrom(instance->fds[i].fd, buf, ETH_FRAME_LEN, 0, \
						(struct sockaddr*)&addr, &addr_len);
				if (len <= 0) {
					log(ERROR, "receive packet error: %s", strerror(errno));
				}
				else if (addr.sll_pkttype == PACKET_OUTGOING) {
					// XXX: Linux raw socket will capture both incoming and
					// outgoing packets, while we only care about the incoming ones.

					// log(DEBUG, "received packet which is sent from the "
					// 		"interface itself, drop it.");
				}
				else {
					iface_info_t *iface = fd_to_iface(instance->fds[i].fd);
					if (!iface) 
						continue;

					char *packet = malloc(len);
					if (!packet) {
						log(ERROR, "malloc failed when receiving packet.");
						continue;
					}
					memcpy(packet, buf, len);
					handle_packet(iface, packet, len);
				}
			}
		}
	}
}

int main(int argc, const char **argv)
{
	if (getuid() && geteuid()) {
		printf("Permission denied, should be superuser!\n");
		exit(1);
	}

	init_ustack();

	init_mac_port_table();

	ustack_run();

	return 0;
}
