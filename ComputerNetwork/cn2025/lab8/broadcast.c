#include "base.h"
#include <stdio.h>

extern ustack_t *instance;

void broadcast_packet(iface_info_t *iface, const char *packet, int len)
{
	// TODO: broadcast packet
	iface_info_t *pos = NULL;
	list_for_each_entry(pos, &instance->iface_list, list) 
	{
		if (pos != iface) 
		{ 
			iface_send_packet(pos, packet, len);
		}
	}
}
