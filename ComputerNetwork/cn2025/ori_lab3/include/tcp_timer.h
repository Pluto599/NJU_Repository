#ifndef __TCP_TIMER_H__
#define __TCP_TIMER_H__

#include "list.h"

#include <stddef.h>

struct tcp_timer
{
    int type;    // time-wait: 0		
                 // retrans: 1
                 // persist: 2
    int timeout; // in micro second
    struct list_head list;
    int enable;
    int retrans_cnt; // 当前累计重传次数
};

struct tcp_sock;
#define timewait_to_tcp_sock(t)      (struct tcp_sock*)((char*)(t) - offsetof(struct tcp_sock, timewait))
#define retranstimer_to_tcp_sock(t)  (struct tcp_sock*)((char*)(t) - offsetof(struct tcp_sock, retrans_timer))
#define persistimer_to_tcp_sock(t)  (struct tcp_sock*)((char*)(t) - offsetof(struct tcp_sock, persist_timer))

#define TCP_TIMER_SCAN_INTERVAL      100000
#define TCP_MSL                      1000000
#define TCP_TIMEWAIT_TIMEOUT         (2 * TCP_MSL)
#define TCP_RETRANS_INTERVAL_INITIAL 200000

// the thread that scans timer_list periodically
void* tcp_timer_thread(void* arg);
// add the timer of tcp sock to timer_list
void tcp_set_timewait_timer(struct tcp_sock*);

// 启用定时器
void tcp_set_retrans_timer(struct tcp_sock* tsk);
// 禁用定时器
void tcp_unset_retrans_timer(struct tcp_sock* tsk);
// 在收到ACK后更新定时器
void tcp_update_retrans_timer(struct tcp_sock* tsk);

/*
Persist Timer：即使发送方被通知接收窗口归零，发送方仍会隔一段时间发送一个Probe报文，它包含一个字节的数据，用这个数据试探新的接收窗口。

本实验中：
1. 窗口小于TCP_MSS时，发送就停止了（参见上面修改的tcp_update_window函数），所以上面的窗口归零在本实验中被替换为，窗口从大于等于TCP_MSS变为小于TCP_MSS。窗口恢复类同。
2. 由于本框架没有通常意义上的发送缓冲区（应用层和协议栈之间的，和上面的send_buf不是一个东西），所以我们无法在协议栈内得知下一个要发送的字节是什么。因而这里对Probe报文稍作修改，修改为发送一个已经ACK过的字节，这样仍然可以正常工作，因为TCP 接收方在接收到一个已经ACK过的字节时，会回复ACK。
*/
// 启用persist timer
void tcp_set_persist_timer(struct tcp_sock *tsk); 
// 禁用persist timer
void tcp_unset_persist_timer(struct tcp_sock *tsk);

#endif
