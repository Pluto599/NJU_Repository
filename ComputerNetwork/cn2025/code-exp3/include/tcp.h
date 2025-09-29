#ifndef __TCP_H__
#define __TCP_H__

#include "checksum.h"
#include "ip.h"
#include "types.h"

#include <endian.h>

#define less_or_equal_32b(a, b)    (((int32_t)(a) - (int32_t)(b)) <= 0)
#define less_than_32b(a, b)        (((int32_t)(a) - (int32_t)(b)) < 0)
#define greater_or_equal_32b(a, b) (((int32_t)(a) - (int32_t)(b)) >= 0)
#define greater_than_32b(a, b)     (((int32_t)(a) - (int32_t)(b)) > 0)

// format of standard tcp header
struct tcphdr
{
    u16 sport; // 源端口
    u16 dport; // 目的端口
    u32 seq;   // 序列号
    u32 ack;   // 确认号
#if __BYTE_ORDER == __LITTLE_ENDIAN
    u8 x2 : 4;  // 保留位(未使用)
    u8 off : 4; // 数据偏移(首部长度)
#elif __BYTE_ORDER == __BIG_ENDIAN
    u8 off : 4; // 数据偏移(首部长度)
    u8 x2 : 4;  // 保留位(未使用)
#endif
    u8 flags; // 标志位
#define TCP_FIN 0x01
#define TCP_SYN 0x02
#define TCP_RST 0x04
#define TCP_PSH 0x08
#define TCP_ACK 0x10
#define TCP_URG 0x20
    u16 rwnd;     // 接收窗口
    u16 checksum; // 检验和
    u16 urp;      // 紧急指针
} __attribute__((packed));

#define TCP_HDR_OFFSET     5
#define TCP_BASE_HDR_SIZE  20
#define TCP_HDR_SIZE(tcp)  (tcp->off * 4)

#define TCP_DEFAULT_WINDOW 65535

// TODO 3.1.2
#define TCP_MSS (ETH_FRAME_LEN - ETHER_HDR_SIZE - IP_BASE_HDR_SIZE - TCP_BASE_HDR_SIZE)

// control block, representing all the necesary information of a packet
struct tcp_cb
{
    u32 saddr;          // 数据包的源地址
    u32 daddr;          // 数据包的源端口
    u16 sport;          // 数据包的目的地址
    u16 dport;          // 数据包的目的端口
    u32 seq;            // TCP头部中的序列号
    u32 seq_end;        // seq + (SYN|FIN) + 负载长度
    u32 ack;            // TCP头部中的确认号
    u32 rwnd;           // TCP头部中的接收窗口
    u8 flags;           // TCP头部中的标志位
    struct iphdr* ip;   // 指向IP头部的指针
    struct tcphdr* tcp; // 指向TCP头部的指针
    char* payload;      // 指向TCP数据的指针
    int pl_len;         // TCP数据的长度
};

// tcp states
enum tcp_state
{
    TCP_CLOSED,
    TCP_LISTEN,
    TCP_SYN_RECV,
    TCP_SYN_SENT,
    TCP_ESTABLISHED,
    TCP_CLOSE_WAIT,
    TCP_LAST_ACK,
    TCP_FIN_WAIT_1,
    TCP_FIN_WAIT_2,
    TCP_CLOSING,
    TCP_TIME_WAIT
};

static inline struct tcphdr* packet_to_tcp_hdr(char* packet)
{
    struct iphdr* ip = packet_to_ip_hdr(packet);
    return (struct tcphdr*)((char*)ip + IP_HDR_SIZE(ip));
}

static inline u16 tcp_checksum(struct iphdr* ip, struct tcphdr* tcp)
{
    u16 tmp = tcp->checksum;
    tcp->checksum = 0;

    u16 reserv_proto = ip->protocol;
    u16 tcp_len = ntohs(ip->tot_len) - IP_HDR_SIZE(ip);

    u32 sum = ip->saddr + ip->daddr + htons(reserv_proto) + htons(tcp_len);
    u16 cksum = checksum((u16*)tcp, (int)tcp_len, sum);

    tcp->checksum = tmp;

    return cksum;
}

extern const char* tcp_state_str[];
static inline const char* tcp_state_to_str(int state)
{
    return tcp_state_str[state];
}

void tcp_copy_flags_to_str(u8 flags, char buf[]);
void tcp_cb_init(struct iphdr* ip, struct tcphdr* tcp, struct tcp_cb* cb);
void handle_tcp_packet(char* packet, struct iphdr* ip, struct tcphdr* tcp);

#endif
