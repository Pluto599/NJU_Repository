#include "tcp.h"
#include "tcp_sock.h"
#include "tcp_timer.h"

#include "log.h"
#include "ring_buffer.h"

#include <stdlib.h>

#define min(x, y) ((x) < (y) ? (x) : (y))

// update the snd_wnd of tcp_sock
// if the snd_wnd before updating is zero, notify tcp_sock_send (wait_send)
// TODO 3.1.3
/*
1. 记录更新前的tcp_tx_window_test结果 
2. 更新snd_una， adv_wnd， snd_wnd。变量含义在tcp_sock结构体注释里。cwnd后面拥塞控制才会用到，可以设置一个较大的值0x7f7f7f7f。 
3. 检查新的tcp_tx_window_test结果 
4. 如果原本没有足够的窗口，现在有了，唤醒tsk->wait_send
*/
static inline void tcp_update_window(struct tcp_sock *tsk, struct tcp_cb *cb)
{
    int old_window = tcp_tx_window_test(tsk);

    tsk->snd_una = cb->ack;
    tsk->adv_wnd = cb->rwnd;
    tsk->cwnd = 0x7f7f7f7f;
    tsk->snd_wnd = min(tsk->adv_wnd, tsk->cwnd);
    int new_window = tcp_tx_window_test(tsk);

    if(old_window == 0 && new_window > 0)
    {
        log(DEBUG, "tcp_update_window: snd_wnd updated from 0 to %u, waking up wait_send.", tsk->snd_wnd);
        wake_up(tsk->wait_send);
        tcp_unset_persist_timer(tsk);
    }
    else
    {
        log(DEBUG, "tcp_update_window: snd_wnd updated to %u.", tsk->snd_wnd);
    }
}

// update the snd_wnd safely: cb->ack should be between snd_una and snd_nxt
static inline void tcp_update_window_safe(struct tcp_sock* tsk, struct tcp_cb* cb)
{
    if (less_or_equal_32b(tsk->snd_una, cb->ack) && less_or_equal_32b(cb->ack, tsk->snd_nxt))
        tcp_update_window(tsk, cb);
}

#ifndef max
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif

// check whether the sequence number of the incoming packet is in the receiving window
static inline int is_tcp_seq_valid(struct tcp_sock* tsk, struct tcp_cb* cb)
{
    u32 rcv_end = tsk->rcv_nxt + max(tsk->rcv_wnd, 1);
    if (less_than_32b(cb->seq, rcv_end) && less_or_equal_32b(tsk->rcv_nxt, cb->seq_end))
    {
        return 1;
    }
    else
    {
        log(ERROR, "received packet with invalid seq, drop it.");
        return 0;
    }
}
// TODO1.3
// Process the incoming packet according to TCP state machine.
// TODO 3.1.4
// 在接收到ACK时调用tcp_update_window_safe。最好snd_una, adv_wnd, snd_wnd这三个值的更新只应该发生在tcp_update_window_safe中，否则可能导致窗口状态判断错误，无法唤醒wait_send信号。 
// 更新接收报文后回复ACK的部分，按照上述机制。
void tcp_process(struct tcp_sock *tsk, struct tcp_cb *cb, char *packet)
{
    if (tsk == NULL)
    {
        log(ERROR, "tcp_process receive packet but tsk is NULL");
        return;
    }
    switch (tsk->state)
    {
        case TCP_CLOSED:
            tcp_send_reset(cb);
            break;

        case TCP_LISTEN:
            if (cb->flags & TCP_SYN)
            {
                struct tcp_sock* child_tsk = alloc_tcp_sock();
                if (!child_tsk)
                {
                    log(ERROR, "Failed to allocate tcp sock for incoming connection.");
                    return;
                }

                child_tsk->sk_sip = cb->daddr;
                child_tsk->sk_sport = cb->dport;
                child_tsk->sk_dip = cb->saddr;
                child_tsk->sk_dport = cb->sport;

                child_tsk->parent = tsk;
                child_tsk->iss = tcp_new_iss();
                child_tsk->snd_nxt = child_tsk->iss;
                child_tsk->rcv_nxt = cb->seq + 1;

                tcp_set_state(child_tsk, TCP_SYN_RECV);
                list_add_tail(&child_tsk->list, &tsk->listen_queue);

                if (tcp_hash(child_tsk) < 0)
                {
                    log(ERROR, "Failed to hash child socket.");
                    free_tcp_sock(child_tsk);
                    return;
                }

                tcp_send_control_packet(child_tsk, TCP_SYN | TCP_ACK);
            }
            else if (cb->flags & TCP_ACK)
            {
                tcp_update_window_safe(tsk, cb);
                tcp_send_reset(cb);
            }
            break;

        case TCP_SYN_SENT:
            log(DEBUG, "Received packet in SYN_SENT state");
            if (cb->flags & (TCP_SYN | TCP_ACK))
            {
                tcp_update_window_safe(tsk, cb);

                tsk->rcv_nxt = cb->seq + 1;
                tcp_set_state(tsk, TCP_ESTABLISHED);
                tcp_send_control_packet(tsk, TCP_ACK);
                log(DEBUG, "tcp_process in SYN_SENT state wake up tsk->wait_connect");
                wake_up(tsk->wait_connect);
            }
            else if (cb->flags & TCP_RST)
            {
                log(ERROR, "tcp_process in SYN_SENT state receive RST");
                // tcp_set_state(tsk, TCP_CLOSED);
                // wake_up(tsk->wait_connect);
            }
            break;

        case TCP_SYN_RECV:
            if (cb->flags & TCP_ACK)
            {
                tcp_update_window_safe(tsk, cb);

                tcp_set_state(tsk, TCP_ESTABLISHED);

                if (tsk->parent)
                {
                    tcp_sock_accept_enqueue(tsk);

                    // log(DEBUG, "tcp_process in SYN_RECV state wake up tsk->parent->wait_accept");
                    wake_up(tsk->parent->wait_accept);
                }
            }
            else if (cb->flags & TCP_RST)
            {
                log(ERROR, "tcp_process in SYN_RECV state receive RST");
                // if (tsk->parent)
                // {
                //     list_delete_entry(&tsk->list);
                //     tcp_set_state(tsk, TCP_CLOSED);
                //     free_tcp_sock(tsk);
                // }
                // else
                // {
                //     tcp_set_state(tsk, TCP_CLOSED);
                // }
            }
            break;

        case TCP_ESTABLISHED:
            int need_ack = 0;

            // 1. 序列号验证
            if (!is_tcp_seq_valid(tsk, cb))
            {
                tcp_send_control_packet(tsk, TCP_ACK);
                break;
            }

            // 2. 处理ACK标志
            if (cb->flags & TCP_ACK)
            {
                tcp_update_window_safe(tsk, cb);

                // 有新数据被确认
                if (less_or_equal_32b(tsk->snd_una, cb->ack) && less_or_equal_32b(cb->ack, tsk->snd_nxt))
                {
                    tcp_unset_retrans_timer(tsk);                    
                }
            }

            // 3. 处理数据部分
            if (cb->pl_len > 0)
            {
                // 处理重复数据包

                // 处理有序到达的数据
                if(cb->seq == tsk->rcv_nxt)
                {
                    log(DEBUG, "Received in-order data, seq: %u, rcv_nxt: %u", cb->seq, tsk->rcv_nxt);

                    int old_rcv_wnd = tsk->rcv_wnd;

                    // 更新rcv_nxt
                    tsk->rcv_nxt = cb->seq_end;

                    // 将数据放入接收缓冲区

                    // 处理可能已缓存的乱序数据

                    // 唤醒接收等待队列

                    // 检查窗口状态变化
                    if (old_rcv_wnd == 0 && tsk->rcv_wnd > 0)
                    {
                        need_ack = 1;
                    }
                }
                // 处理乱序到达的数据
                else if (cb->seq > tsk->rcv_nxt)
                {
                    log(DEBUG, "Received out-of-order data, seq: %u, rcv_nxt: %u", cb->seq, tsk->rcv_nxt);

                    // 将数据放入乱序接收缓冲区

                    // 检查是否有缓存的乱序数据可以处理

                }
                // 处理部分重叠的数据

                // 标记需要发送ACK
                need_ack = 1;
            }

            // 4. 处理FIN标志
            if (cb->flags & TCP_FIN && cb->seq == tsk->rcv_nxt)
            {
                tsk->rcv_nxt = cb->seq_end;
                tcp_set_state(tsk, TCP_CLOSE_WAIT);
                tcp_send_control_packet(tsk, TCP_ACK);

                log(DEBUG, "tcp_process in TCP_ESTABLISHED state wake up tsk->wait_recv");
                wake_up(tsk->wait_recv);

                // 标记需要发送ACK
                need_ack = 1;
            }

            // 5. 检查是否需要发送ACK
            if(need_ack)
            {
                // 发送ACK响应
                tcp_send_control_packet(tsk, TCP_ACK);
            }

            break;

        case TCP_FIN_WAIT_1:
            if (cb->flags & TCP_ACK)
            {
                tcp_update_window_safe(tsk, cb);

                if (cb->flags & TCP_FIN)
                {
                    tsk->rcv_nxt = cb->seq_end;
                    tcp_set_state(tsk, TCP_TIME_WAIT);
                    tcp_send_control_packet(tsk, TCP_ACK);
                    tcp_set_timewait_timer(tsk);
                }
                else
                {
                    tcp_set_state(tsk, TCP_FIN_WAIT_2);
                }
            }
            else if (cb->flags & TCP_FIN)
            {
                tsk->rcv_nxt = cb->seq_end;
                tcp_set_state(tsk, TCP_CLOSING);
                tcp_send_control_packet(tsk, TCP_ACK);
            }
            break;

        case TCP_FIN_WAIT_2:
            if (cb->flags & TCP_FIN)
            {
                tsk->rcv_nxt = cb->seq_end;
                tcp_set_state(tsk, TCP_TIME_WAIT);
                tcp_send_control_packet(tsk, TCP_ACK);
                tcp_set_timewait_timer(tsk);
            }
            break;

        case TCP_LAST_ACK:
            if (cb->flags & TCP_ACK)
            {
                tcp_update_window_safe(tsk, cb);

                tcp_set_state(tsk, TCP_CLOSED);
                tcp_unhash(tsk);
            }
            break;

        case TCP_CLOSING:
            if (cb->flags & TCP_ACK)
            {
                tcp_update_window_safe(tsk, cb);

                tcp_set_state(tsk, TCP_TIME_WAIT);
                tcp_set_timewait_timer(tsk);
            }
            break;

        case TCP_TIME_WAIT:
            if (cb->flags & TCP_FIN)
            {
                tcp_send_control_packet(tsk, TCP_ACK);
                tcp_set_timewait_timer(tsk);
            }
            break;

        default:
            // 处理未知状态
            log(ERROR, "Unknown TCP state %d", tsk->state);
            break;
    }
}