#include "ether.h"
#include "ip.h"
#include "tcp.h"
#include "tcp_sock.h"

#include "list.h"
#include "log.h"

#include <stdlib.h>
#include <string.h>

// initialize tcp header according to the arguments
static void tcp_init_hdr(struct tcphdr* tcp, u16 sport, u16 dport, u32 seq, u32 ack, u8 flags, u16 rwnd)
{
    memset((char*)tcp, 0, TCP_BASE_HDR_SIZE);

    tcp->sport = htons(sport);
    tcp->dport = htons(dport);
    tcp->seq = htonl(seq);
    tcp->ack = htonl(ack);
    tcp->off = TCP_HDR_OFFSET;
    tcp->flags = flags;
    tcp->rwnd = htons(rwnd);
}

// send a tcp packet
//
// Given that the payload of the tcp packet has been filled, initialize the tcp
// header and ip header (remember to set the checksum in both header), and emit
// the packet by calling ip_send_packet.
void tcp_send_packet(struct tcp_sock* tsk, char* packet, int len)
{
    struct iphdr* ip = packet_to_ip_hdr(packet);
    struct tcphdr* tcp = (struct tcphdr*)((char*)ip + IP_BASE_HDR_SIZE);

    int ip_tot_len = len - ETHER_HDR_SIZE;
    int tcp_data_len = ip_tot_len - IP_BASE_HDR_SIZE - TCP_BASE_HDR_SIZE;

    u32 saddr = tsk->sk_sip;
    u32 daddr = tsk->sk_dip;
    u16 sport = tsk->sk_sport;
    u16 dport = tsk->sk_dport;

    u32 seq = tsk->snd_nxt;
    u32 ack = tsk->rcv_nxt;
    u16 rwnd = tsk->rcv_wnd;

    tcp_init_hdr(tcp, sport, dport, seq, ack, TCP_PSH | TCP_ACK, rwnd);
    ip_init_hdr(ip, saddr, daddr, ip_tot_len, IPPROTO_TCP);

    tcp->checksum = tcp_checksum(ip, tcp);
    ip->checksum = ip_checksum(ip);

    // 发送数据报文时，加入发送缓冲区并设置重传计时器
    if (tcp_data_len > 0)
    {
        tcp_send_buffer_add_packet(tsk, packet, len, tsk->snd_nxt, tsk->snd_nxt + tcp_data_len);
        tcp_set_retrans_timer(tsk);
    }

    log(DEBUG, "tcp_send_packet: seq = %u, snd_nxt is %u", tsk->snd_nxt - tsk->iss, tsk->snd_nxt + tcp_data_len - tsk->iss);

    tsk->snd_nxt += tcp_data_len;

    ip_send_packet(packet, len);
}

// send a tcp control packet
//
// The control packet is like TCP_ACK, TCP_SYN, TCP_FIN (excluding TCP_RST).
// All these packets do not have payload and the only difference among these is
// the flags.
void tcp_send_control_packet(struct tcp_sock* tsk, u8 flags)
{
    int pkt_size = ETHER_HDR_SIZE + IP_BASE_HDR_SIZE + TCP_BASE_HDR_SIZE;
    char* packet = malloc(pkt_size);
    if (!packet)
    {
        log(ERROR, "malloc tcp control packet failed.");
        return;
    }

    struct iphdr* ip = packet_to_ip_hdr(packet);
    struct tcphdr* tcp = (struct tcphdr*)((char*)ip + IP_BASE_HDR_SIZE);

    u16 tot_len = IP_BASE_HDR_SIZE + TCP_BASE_HDR_SIZE;

    ip_init_hdr(ip, tsk->sk_sip, tsk->sk_dip, tot_len, IPPROTO_TCP);
    tcp_init_hdr(tcp, tsk->sk_sport, tsk->sk_dport, tsk->snd_nxt, tsk->rcv_nxt, flags, tsk->rcv_wnd);

    tcp->checksum = tcp_checksum(ip, tcp);

    // 发送SYN/FIN报文时，加入发送缓冲区并设置重传计时器
    if ((flags & TCP_SYN) || (flags & TCP_FIN))
    {
        log(DEBUG, "tcp_send_control_packet: add FIN or SYN packet to send buffer");
        tcp_send_buffer_add_packet(tsk, packet, pkt_size, tsk->snd_nxt, tsk->snd_nxt + 1);
        tcp_set_retrans_timer(tsk);
        
        tsk->snd_nxt += 1;
    }

    ip_send_packet(packet, pkt_size);
}

// send tcp reset packet
//
// Different from tcp_send_control_packet, the fields of reset packet is
// from tcp_cb instead of tcp_sock.
void tcp_send_reset(struct tcp_cb* cb)
{
    int pkt_size = ETHER_HDR_SIZE + IP_BASE_HDR_SIZE + TCP_BASE_HDR_SIZE;
    char* packet = malloc(pkt_size);
    if (!packet)
    {
        log(ERROR, "malloc tcp control packet failed.");
        return;
    }

    struct iphdr* ip = packet_to_ip_hdr(packet);
    struct tcphdr* tcp = (struct tcphdr*)((char*)ip + IP_BASE_HDR_SIZE);

    u16 tot_len = IP_BASE_HDR_SIZE + TCP_BASE_HDR_SIZE;
    ip_init_hdr(ip, cb->daddr, cb->saddr, tot_len, IPPROTO_TCP);
    tcp_init_hdr(tcp, cb->dport, cb->sport, 0, cb->seq_end, TCP_RST | TCP_ACK, 0);
    tcp->checksum = tcp_checksum(ip, tcp);

    ip_send_packet(packet, pkt_size);
}

int tcp_send_data(struct tcp_sock* tsk, char* buf, int len)
{
    if (!tsk || !buf || len <= 0)
    {
        log(ERROR, "invalid parameters in tcp_send_data");
        return -1;
    }

    int pkt_size = ETHER_HDR_SIZE + IP_BASE_HDR_SIZE + TCP_BASE_HDR_SIZE + len;
    char* packet = malloc(pkt_size);
    if (!packet)
    {
        log(ERROR, "malloc tcp data packet failed.");
        return -1;
    }

    struct iphdr* ip = packet_to_ip_hdr(packet);
    struct tcphdr* tcp = (struct tcphdr*)((char*)ip + IP_BASE_HDR_SIZE);

    char* data = (char*)tcp + TCP_BASE_HDR_SIZE;

    memcpy(data, buf, len);

    pthread_mutex_lock(&tsk->sk_lock); 
    tcp_send_packet(tsk, packet, pkt_size);
    pthread_mutex_unlock(&tsk->sk_lock); 

    return len;
}

// TODO 3.2.3
// 发送probe报文
/*
仿照tcp_send_packet函数，发送probe报文。几处改动：
1. 发送的序列号设置为一个已经ACK过的序列号（比如tsk->snd_una - 1）
2. 不需要更新snd_nxt
3. 不需要设置重传相关内容
4. TCP负载为一个任意的字节 --> change:没有负载
*/
void tcp_send_probe_packet(struct tcp_sock *tsk)
{
    log(DEBUG, "tcp_send_probe_packet");

    int pkt_size = ETHER_HDR_SIZE + IP_BASE_HDR_SIZE + TCP_BASE_HDR_SIZE + 1; // 0 byte payload ?
    char *packet = malloc(pkt_size);
    if (!packet)
    {
        log(ERROR, "malloc tcp probe packet failed.");
        return;
    }

    struct iphdr *ip = packet_to_ip_hdr(packet);
    struct tcphdr *tcp = (struct tcphdr *)((char *)ip + IP_BASE_HDR_SIZE);

    int ip_tot_len = pkt_size - ETHER_HDR_SIZE;

    u32 saddr = tsk->sk_sip;
    u32 daddr = tsk->sk_dip;
    u16 sport = tsk->sk_sport;
    u16 dport = tsk->sk_dport;

    u32 seq = tsk->iss;
    u32 ack = tsk->rcv_nxt;
    u16 rwnd = tsk->rcv_wnd;

    tcp_init_hdr(tcp, sport, dport, seq, ack, TCP_PSH | TCP_ACK, rwnd);
    ip_init_hdr(ip, saddr, daddr, ip_tot_len, IPPROTO_TCP);

    tcp->checksum = tcp_checksum(ip, tcp);
    ip->checksum = ip_checksum(ip);

    ip_send_packet(packet, pkt_size);
}

// TODO 3.3.3
/*
创建send_buffer_entry加入send_buf尾部
注意上锁，后面不再强调。

调用同tcp_set_retrans_timer。
*/
void tcp_send_buffer_add_packet(struct tcp_sock *tsk, char *packet, int len, u32 seq, u32 seq_end)
{
    pthread_mutex_lock(&tsk->send_buf_lock);

    log(DEBUG, "tcp_send_buffer_add_packet: seq = %u, seq_end = %u", seq - tsk->iss, seq_end - tsk->iss);
    struct send_buffer_entry *entry = malloc(sizeof(struct send_buffer_entry));
    if (!entry)
    {
        log(ERROR, "malloc send buffer entry failed");
        pthread_mutex_unlock(&tsk->send_buf_lock);
        return;
    }

    entry->packet = malloc(len);
    if (!entry->packet)
    {
        log(ERROR, "malloc packet buffer failed");
        free(entry);
        pthread_mutex_unlock(&tsk->send_buf_lock);
        return;
    }
    memcpy(entry->packet, packet, len);

    entry->len = len;
    entry->seq = seq;
    entry->seq_end = seq_end;

    list_add_tail(&entry->list, &tsk->send_buf);

    pthread_mutex_unlock(&tsk->send_buf_lock);
}

/*
基于收到的ACK包，遍历发送队列，将已经接收的数据包从队列中移除
提取报文的tcp头可以使用packet_to_tcp_hdr，注意报文中的字段是大端序

收到ACK时调用。与tcp_update_retrans_timer不同，还包含了SYN、FIN的ACK。
*/
int tcp_update_send_buffer(struct tcp_sock *tsk, u32 ack)
{
    int count = 0;

    pthread_mutex_lock(&tsk->send_buf_lock);

    struct send_buffer_entry *entry, *q;
    list_for_each_entry_safe(entry, q, &tsk->send_buf, list)
    {
        if (less_or_equal_32b(entry->seq_end, ack))
        {
            list_delete_entry(&entry->list);
            free(entry->packet);
            free(entry);
            count++;
        }
    }

    pthread_mutex_unlock(&tsk->send_buf_lock);
    return count;
}

/*
获取重传队列第一个包，修改ack号和checksum并通过ip_send_packet发送。
注意不要更新snd_nxt之类的参数，这是一个独立的重传报文。

在重传定时器超时时调用。
*/
int tcp_retrans_send_buffer(struct tcp_sock *tsk)
{
    log(DEBUG, "tcp_retrans_send_buffer");

    pthread_mutex_lock(&tsk->send_buf_lock);

    if (list_empty(&tsk->send_buf))
    {
        pthread_mutex_unlock(&tsk->send_buf_lock);
        return -1;
    }

    struct send_buffer_entry *entry = list_entry(tsk->send_buf.next, struct send_buffer_entry, list);

    char *packet_copy = malloc(entry->len);
    if (!packet_copy)
    {
        log(ERROR, "malloc packet copy failed");
        pthread_mutex_unlock(&tsk->send_buf_lock);
        return -1;
    }
    memcpy(packet_copy, entry->packet, entry->len);

    struct iphdr *ip = packet_to_ip_hdr(packet_copy);
    struct tcphdr *tcp = packet_to_tcp_hdr(packet_copy);

    tcp->ack = htonl(tsk->rcv_nxt);
    ip->checksum = ip_checksum(ip);
    tcp->checksum = tcp_checksum(ip, tcp);

    // 发送报文拷贝
    log(DEBUG, "Retransmit packet seq = %u", entry->seq - tsk->iss);
    ip_send_packet(packet_copy, entry->len);

    pthread_mutex_unlock(&tsk->send_buf_lock);
    return 0;
}