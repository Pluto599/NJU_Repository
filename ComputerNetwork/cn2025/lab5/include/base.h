#ifndef __BASE_H__
#define __BASE_H__

#include "types.h"
#include "ether.h"
#include "list.h"

#include <unistd.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <poll.h>
#include <ifaddrs.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/route.h>
#include <net/if.h>

#include <linux/if_packet.h>
#include <linux/udp.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

typedef struct {
	struct list_head iface_list;	// the list of interfaces
	int nifs;						// number of interfaces
	struct pollfd *fds;				// structure used to poll packets among 
								    // all the interfaces
} ustack_t;

extern ustack_t *instance;

typedef struct {
	struct list_head list; // list node used to link all interfaces 链表节点，连接所有接口，在本实验中不重要

	int fd;						// file descriptor for receiving & sending packets 文件描述符，在本实验中不重要
	int index;					// the index (unique ID) of this interface 接口的unique ID
	u8 mac[ETH_ALEN];			// mac address of this interface 接口的MAC地址
	u32 ip;						// IPv4 address (in host byte order) 接口的IP地址(主机序)
	u32 mask;					// Network Mask (in host byte order) 子网掩码 (主机序)
	char name[16];				// name of this interface 接口名称
	char ip_str[16];			// readable IP address 可读的IP地址
} iface_info_t;

void init_ustack();
iface_info_t *fd_to_iface(int fd);
void iface_send_packet(iface_info_t *iface, const char *packet, int len);
#endif
