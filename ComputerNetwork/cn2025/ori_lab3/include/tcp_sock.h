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
    // sk_ip, sk_sport, sk_sip, sk_dport are the 4-tuple that represents a connection
    // 本地地址
    struct sock_addr local;
    // 对端地址
    struct sock_addr peer;
    
// 源ip地址
#define sk_sip   local.ip
// 源端口
#define sk_sport local.port
// 目的ip地址
#define sk_dip   peer.ip
// 目的端口
#define sk_dport peer.port

    // 引用计数 & 哈希链表
    // pointer to parent tcp sock, a tcp sock which bind and listen to a port is the parent of tcp socks when *accept* a connection request
    // 父socket（用于接受连接的监听socket）
    struct tcp_sock* parent;
    
    // represents the number that the tcp sock is referred, if this number decreased to zero, the tcp sock should be released
    // 引用计数
    int ref_cnt;
    
    // hash_list is used to hash tcp sock into listen_table or established_table, bind_hash_list is used to hash into bind_table
    // 用于建立/监听表的哈希链表节点
    struct list_head hash_list;
    // 用于绑定表的哈希链表节点
    struct list_head bind_hash_list;

    // 连接队列
    // when a passively opened tcp sock receives a SYN packet, it mallocs a child
    // tcp sock to serve the incoming connection, which is pending in the
    // listen_queue of parent tcp sock
    // 半连接队列（SYN_RECV状态的连接）
    struct list_head listen_queue;
    // when receiving the last packet (ACK) of the 3-way handshake, the tcp sock
    // in listen_queue will be moved into accept_queue, waiting for *accept* by
    // parent tcp sock
    // 全连接队列（已完成三次握手的连接）
    struct list_head accept_queue;

#define TCP_MAX_BACKLOG 128

    // the number of pending tcp sock in accept_queue
    // 当前accept队列长度
    int accept_backlog;
    // the maximum number of pending tcp sock in accept_queue
    // accept队列最大长度
    int backlog;
    // the list node used to link listen_queue or accept_queue of parent tcp sock
    // 链表节点（用于连接队列）
    struct list_head list;

    // 定时器
    // tcp timer used during TCP_TIME_WAIT state
    // TIME_WAIT状态定时器
    struct tcp_timer timewait;
    // used for timeout retransmission
    // 重传定时器
    struct tcp_timer retrans_timer;
    // TODO 3.2
    // persist timer
    struct tcp_timer persist_timer;

    // 线程同步等待
    // synch waiting structure of *connect*, *accept*, *recv*, and *send*
    // 连接等待
    struct synch_wait* wait_connect;
    // 接受等待
    struct synch_wait* wait_accept;
    // 接收等待
    struct synch_wait* wait_recv;
    // 发送等待
    struct synch_wait* wait_send;

    // 缓冲区
    // receiving buffer
    // 接收缓冲区
    struct ring_buffer* rcv_buf;
    // 发送缓冲区（未确认的数据）
    struct list_head send_buf;
    // 乱序接收缓冲区
    struct list_head rcv_ofo_buf;

    // TCP状态
    // tcp state, see enum tcp_state in tcp.h
    // TCP连接状态
    int state;

    // 序列号
    // initial sending sequence number
    // 初始发送序列号
    u32 iss;
    // the highest byte that is ACKed by peer
    // 最低未确认序列号
    u32 snd_una;
    // the highest byte sent
    // 下一个发送序列号
    u32 snd_nxt;
    // the highest byte ACKed by itself (i.e. the byte expected to receive next)
    // 期望接收的下一个序列号
    u32 rcv_nxt;
    // used to indicate the end of fast recovery
    // 快速恢复结束点
    u32 recovery_point;

    // 流量控制 & 拥塞控制
    // min(adv_wnd, cwnd)
    // 发送窗口大小
    u32 snd_wnd;
    // the receiving window advertised by peer
    // 对端接收窗口大小
    u16 adv_wnd;
    // the size of receiving window (advertised by tcp sock itself)
    // 本地接收窗口大小
    u16 rcv_wnd;
    // congestion window
    // 拥塞窗口大小
    u32 cwnd;
    // slow start threshold
    // 慢启动阈值
    u32 ssthresh;

    // TODO 3.1.1
    // 保护snd_una等核心参数
    // 收包时：在tcp_process调用之前上锁，之后解锁。这样整个收包过程对socket参数的更改都是安全的。
    // 发包时：在tcp_send_packet调用之前上锁，之后解锁。这里由于每个人实现不一样，加锁位置不一定一样，比如在调用tcp_send_packet之前，用tcp_tx_window_test检查发送窗口，那么应该在tcp_tx_window_test之前上锁。
    pthread_mutex_t sk_lock;

    // 保护struct ring_buffer *rcv_buf
    // 每次访问rcv_buf时
    pthread_mutex_t rcv_buf_lock;

    // 保护struct list_head send_buf;
    // 每次访问send_buf时
    pthread_mutex_t send_buf_lock; 
};

// 发送队列条目，用于存储已发送但未确认的报文
// 创建这个结构时应当拷贝报文信息，而非直接复制指针，防止重复释放。
struct send_buffer_entry
{
    struct list_head list; // 链表节点
    char *packet;          // 报文数据的副本
    int len;               // 报文长度
    u32 seq;               // 起始序列号
    u32 seq_end;           // 结束序列号
};

// 接收乱序队列条目，用于存储乱序到达的报文
struct recv_ofo_buf_entry
{
    struct list_head list; // 链表节点
    char *data;            // 报文数据
    int len;               // 数据长度
    u32 seq;               // 起始序列号
    u32 seq_end;           // 结束序列号
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
// 发送probe报文
void tcp_send_probe_packet(struct tcp_sock *tsk);

void tcp_process(struct tcp_sock* tsk, struct tcp_cb* cb, char* packet);

void init_tcp_stack();

int tcp_sock_bind(struct tcp_sock* tsk, struct sock_addr* skaddr);
int tcp_sock_listen(struct tcp_sock* tsk, int backlog);
int tcp_sock_connect(struct tcp_sock* tsk, struct sock_addr* skaddr);
struct tcp_sock* tcp_sock_accept(struct tcp_sock* tsk);
void tcp_sock_close(struct tcp_sock* tsk);
// TODO 2.2.1 done
int tcp_sock_read(struct tcp_sock* tsk, char* buf, int len);
int tcp_sock_write(struct tcp_sock* tsk, char* buf, int len);

int tcp_tx_window_test(struct tcp_sock *tsk);

/*
重传机制：

发送端：发送数据报文、FIN、SYN报文后将报文加入send_buf中，ACK报文到达时会将已经ACK的报文从send_buf中移除，重传定时器超时则会重发第一个报文。

接收端：接收到数据报文后，检查序列号后将报文放入rcv_ofo_buf，然后查看rcv_ofo_buf中是否有连续有效的数据报文，如果有则上送接收缓冲区rcv_buf。
*/

// 在发送报文时，将其加入发送队列。
void tcp_send_buffer_add_packet(struct tcp_sock *tsk, char *packet, int len, u32 seq, u32 seq_end);
// 在收到ACK时，更新发送队列。
int tcp_update_send_buffer(struct tcp_sock *tsk, u32 ack);
// 在重传定时器超时后，重传发送队列的第一个包。
int tcp_retrans_send_buffer(struct tcp_sock *tsk);

// 将收到的所有数据包加入乱序队列。
int tcp_recv_ofo_buffer_add_packet(struct tcp_sock *tsk, struct tcp_cb *cb);
// 将乱序队列中的有序部分上送接收缓冲区
int tcp_move_recv_ofo_buffer(struct tcp_sock *tsk);

#endif
