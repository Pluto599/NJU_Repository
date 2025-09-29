#ifndef __TCP_SOCK_H__
#define __TCP_SOCK_H__

#include "list.h"
#include "ring_buffer.h"
#include "tcp.h"
#include "tcp_timer.h"
#include "types.h"

#include "synch_wait.h"

#include <pthread.h>

#define PORT_MIN 12345
#define PORT_MAX 23456

struct sock_addr
{
    u32 ip;
    u16 port;
} __attribute__((packed));

// the main structure that manages a connection locally
struct tcp_sock
{
    // 连接标识信息
    // sk_ip, sk_sport, sk_sip, sk_dport are the 4-tuple that represents a
    // connection
    struct sock_addr local; // 本地地址
    struct sock_addr peer;  // 对端地址
#define sk_sip   local.ip   // 源ip地址
#define sk_sport local.port // 源端口
#define sk_dip   peer.ip    // 目的ip地址
#define sk_dport peer.port  // 目的端口

    // 引用计数 & 哈希链表
    // pointer to parent tcp sock, a tcp sock which bind and listen to a port
    // is the parent of tcp socks when *accept* a connection request
    struct tcp_sock* parent; // 父socket（用于接受连接的监听socket）

    // represents the number that the tcp sock is referred, if this number
    // decreased to zero, the tcp sock should be released
    int ref_cnt; // 引用计数

    // hash_list is used to hash tcp sock into listen_table or established_table,
    // bind_hash_list is used to hash into bind_table
    struct list_head hash_list;      // 用于建立/监听表的哈希链表节点
    struct list_head bind_hash_list; // 用于绑定表的哈希链表节点

    // 连接队列
    // when a passively opened tcp sock receives a SYN packet, it mallocs a child
    // tcp sock to serve the incoming connection, which is pending in the
    // listen_queue of parent tcp sock
    struct list_head listen_queue; // 半连接队列（SYN_RECV状态的连接）
    // when receiving the last packet (ACK) of the 3-way handshake, the tcp sock
    // in listen_queue will be moved into accept_queue, waiting for *accept* by
    // parent tcp sock
    struct list_head accept_queue; // 全连接队列（已完成三次握手的连接）

#define TCP_MAX_BACKLOG 128

    // the number of pending tcp sock in accept_queue
    int accept_backlog; // 当前accept队列长度
    // the maximum number of pending tcp sock in accept_queue
    int backlog; // accept队列最大长度
    // the list node used to link listen_queue or accept_queue of parent tcp sock
    struct list_head list; // 链表节点（用于连接队列）

    // 定时器
    // tcp timer used during TCP_TIME_WAIT state
    struct tcp_timer timewait; // TIME_WAIT状态定时器
    // used for timeout retransmission
    struct tcp_timer retrans_timer; // 重传定时器

    // 线程同步等待
    // synch waiting structure of *connect*, *accept*, *recv*, and *send*
    struct synch_wait* wait_connect; // 连接等待
    struct synch_wait* wait_accept;  // 接受等待
    struct synch_wait* wait_recv;    // 接收等待
    struct synch_wait* wait_send;    // 发送等待

    // 缓冲区
    // receiving buffer
    struct ring_buffer* rcv_buf;  // 接收缓冲区
    struct list_head send_buf;    // 发送缓冲区（未确认的数据）
    struct list_head rcv_ofo_buf; // 乱序接收缓冲区

    // TCP状态
    // tcp state, see enum tcp_state in tcp.h
    int state; // TCP连接状态

    // 序列号
    // initial sending sequence number
    u32 iss; // 初始发送序列号
    // the highest byte that is ACKed by peer
    u32 snd_una; // 最低未确认序列号
    // the highest byte sent
    u32 snd_nxt; // 下一个发送序列号
    // the highest byte ACKed by itself (i.e. the byte expected to receive next)
    u32 rcv_nxt; // 期望接收的下一个序列号
    // used to indicate the end of fast recovery
    u32 recovery_point; // 快速恢复结束点

    // 流量控制 & 拥塞控制
    // min(adv_wnd, cwnd)
    u32 snd_wnd; // 发送窗口大小
    // the receiving window advertised by peer
    u16 adv_wnd; // 对端接收窗口大小
    // the size of receiving window (advertised by tcp sock itself)
    u16 rcv_wnd; // 本地接收窗口大小
    // congestion window
    u32 cwnd; // 拥塞窗口大小
    // slow start threshold
    u32 ssthresh; // 慢启动阈值
};

void tcp_set_state(struct tcp_sock* tsk, int state);

int tcp_sock_accept_queue_full(struct tcp_sock* tsk);
void tcp_sock_accept_enqueue(struct tcp_sock* tsk);
struct tcp_sock* tcp_sock_accept_dequeue(struct tcp_sock* tsk);

int tcp_hash(struct tcp_sock* tsk);
void tcp_unhash(struct tcp_sock* tsk);
void tcp_bind_unhash(struct tcp_sock* tsk);
struct tcp_sock* alloc_tcp_sock();
void free_tcp_sock(struct tcp_sock* tsk);
struct tcp_sock* tcp_sock_lookup(struct tcp_cb* cb);

u32 tcp_new_iss();

void tcp_send_reset(struct tcp_cb* cb);

void tcp_send_control_packet(struct tcp_sock* tsk, u8 flags);
void tcp_send_packet(struct tcp_sock* tsk, char* packet, int len);
int tcp_send_data(struct tcp_sock* tsk, char* buf, int len);

void tcp_process(struct tcp_sock* tsk, struct tcp_cb* cb, char* packet);

void init_tcp_stack();

int tcp_sock_bind(struct tcp_sock* tsk, struct sock_addr* skaddr);
int tcp_sock_listen(struct tcp_sock* tsk, int backlog);
int tcp_sock_connect(struct tcp_sock* tsk, struct sock_addr* skaddr);
struct tcp_sock* tcp_sock_accept(struct tcp_sock* tsk);
void tcp_sock_close(struct tcp_sock* tsk);
// TODO2.1
int tcp_sock_read(struct tcp_sock* tsk, char* buf, int len);
int tcp_sock_write(struct tcp_sock* tsk, char* buf, int len);

#endif
