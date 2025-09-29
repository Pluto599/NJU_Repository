#include "tcp.h"
#include "tcp_sock.h"
#include "tcp_timer.h"

#include "log.h"
#include "ring_buffer.h"

#include <stdlib.h>

void log_tcp_event(struct tcp_sock *tsk, const char *event_desc);

// update the snd_wnd of tcp_sock
//
// if the snd_wnd before updating is zero, notify tcp_sock_send (wait_send)
// TODO 3.1.3
/*
1. 记录更新前的tcp_tx_window_test结果
2. 更新snd_una， adv_wnd， snd_wnd。变量含义在tcp_sock结构体注释里。cwnd后面拥塞控制才会用到，可以设置一个较大的值0x7f7f7f7f。
3. 检查新的tcp_tx_window_test结果
4. 如果原本没有足够的窗口，现在有了，唤醒tsk->wait_send
*/
// TODO 3.2.5
/*
修改tcp_update_window函数。
比较简单的修改方式是，只要新的snd_wnd小于TCP_MSS，就调用tcp_set_persist_timer，否则调用tcp_unset_persist_timer，这两个函数在已经启用或禁用时不会重复操作。
当然也可以加些判断，只在归零和恢复的时候调用这两个函数。
*/
// TODO 4
static inline void tcp_update_window(struct tcp_sock* tsk, struct tcp_cb* cb)
{
    int old_test = tcp_tx_window_test(tsk);

    tsk->snd_una = cb->ack;
    tsk->adv_wnd = cb->rwnd;
    log(DEBUG, "cb->rwnd = %u", cb->rwnd);
    // tsk->cwnd = 0x7f7f7f7f;
    tsk->snd_wnd = min(tsk->adv_wnd,tsk->cwnd);

    int new_test = tcp_tx_window_test(tsk);

    if (new_test == 0)
    {
        tcp_set_persist_timer(tsk);
    }
    else
    {
        tcp_unset_persist_timer(tsk);
    }

    if(old_test == 0 && new_test > 0)
    {
        log(DEBUG, "tcp_update_window: snd_wnd is updated, wake up tsk->wait_send");
        wake_up(tsk->wait_send);
        log(DEBUG, "current useable window size: %u", tsk->snd_una + tsk->snd_wnd - tsk->snd_nxt);
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
        log(DEBUG, "seq = %u, seq_end = %u, rcv_nxt = %u, rcv_end = %u", cb->seq - tsk->iss, cb->seq_end - tsk->iss, tsk->rcv_nxt - tsk->iss, rcv_end - tsk->iss);

        log(ERROR, "received packet with invalid seq %u, pl->len = %d, drop it.", cb->seq - tsk->iss, cb->pl_len);
        return 0;
    }
}

// Process the incoming packet according to TCP state machine.
// TODO 2.1.3 done
// TODO 3.1.4
/*
1. 在接收到ACK时调用tcp_update_window_safe。最好snd_una, adv_wnd, snd_wnd这三个值的更新只应该发生在tcp_update_window_safe中，否则可能导致窗口状态判断错误，无法唤醒wait_send信号。
2. 更新接收报文后回复ACK的部分，按照上述机制。
*/
void tcp_process(struct tcp_sock* tsk, struct tcp_cb* cb, char* packet)
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
                tcp_update_send_buffer(tsk, cb->ack);

                tcp_congestion_control(tsk, cb, packet);

                tcp_send_reset(cb);
            }
            break;
            
            case TCP_SYN_RECV:
            if (cb->flags & TCP_ACK)
            {
                tcp_update_window_safe(tsk, cb);
                tcp_update_send_buffer(tsk, cb->ack);

                tcp_congestion_control(tsk, cb, packet);

                tcp_set_state(tsk, TCP_ESTABLISHED);
                tcp_unset_retrans_timer(tsk);
                
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

        case TCP_SYN_SENT:
            if (cb->flags & (TCP_SYN | TCP_ACK))
            {
                tcp_update_window_safe(tsk, cb);
                tcp_update_send_buffer(tsk, cb->ack);

                tcp_congestion_control(tsk, cb, packet);

                tsk->rcv_nxt = cb->seq + 1;

                tcp_set_state(tsk, TCP_ESTABLISHED);
                tcp_send_control_packet(tsk, TCP_ACK);
                tcp_unset_retrans_timer(tsk);

                // 对客户端创建cwnd记录线程
                if (tsk->sk_sip != tsk->sk_dip)
                {
                    pthread_t cwnd_record;
                    pthread_create(&cwnd_record, NULL, tcp_cwnd_thread, (void *)tsk);
                }

                // log(DEBUG, "tcp_process in SYN_SENT state wake up tsk->wait_connect");
                wake_up(tsk->wait_connect);
            }
            else if (cb->flags & TCP_RST)
            {
                log(ERROR, "tcp_process in SYN_SENT state receive RST");
                // tcp_set_state(tsk, TCP_CLOSED);
                // wake_up(tsk->wait_connect);
            }
            break;

        case TCP_ESTABLISHED:
            log(DEBUG, "tcp_process in TCP_ESTABLISHED state");

            // 记录是否为探测包的标志
            int is_probe_packet = (less_than_32b(cb->seq, tsk->rcv_nxt) && cb->pl_len == 0);

            // 处理探测包
            if (is_probe_packet)
            {
                log(DEBUG, "tcp_process in TCP_ESTABLISHED state receive probe packet");
                log(DEBUG, "seq = %u, cb->pl_len = %d", cb->seq - tsk->iss, cb->pl_len);

                // 回应探测包
                tcp_send_control_packet(tsk, TCP_ACK);
            }
            // 验证序列号是否有效（只对非探测包检查）
            else if (!is_tcp_seq_valid(tsk, cb))
            {
                log(ERROR, "tcp_process in TCP_ESTABLISHED state receive invalid seq");
                tcp_send_control_packet(tsk, TCP_ACK);
                break;
            }

            // 处理纯ACK包，排除已处理的探测包
            if (!is_probe_packet && (cb->flags & TCP_ACK) && !(cb->flags & TCP_FIN) && cb->pl_len == 0)
            {
                log(DEBUG, "tcp_process in TCP_ESTABLISHED state receive pure ACK");

                tcp_update_window_safe(tsk, cb);
                tcp_update_send_buffer(tsk, cb->ack);
                tcp_update_retrans_timer(tsk);

                tcp_congestion_control(tsk, cb, packet);

                log(DEBUG, "snd_wnd = %u", tsk->snd_wnd);
            }

            if (cb->pl_len > 0)
            {
                log(DEBUG, "tcp_process in TCP_ESTABLISHED state receive data");
                log(DEBUG, "seq = %u, cb->pl_len = %d", cb->seq - tsk->iss, cb->pl_len);

                // 1: 完全重复的数据包
                if (less_than_32b(cb->seq_end, tsk->rcv_nxt))
                {
                    log(DEBUG, "tcp_process in TCP_ESTABLISHED state receive completely duplicated data packet");
                    tcp_send_control_packet(tsk, TCP_ACK);
                }
                // 2: 按序到达的数据包
                else if (cb->seq == tsk->rcv_nxt)
                {
                    log(DEBUG, "tcp_process in TCP_ESTABLISHED state receive in-order packet");
                    // 将数据包添加到接收缓冲区
                    tcp_recv_ofo_buffer_add_packet(tsk, cb);
                    tcp_send_control_packet(tsk, TCP_ACK);

                    // int ring_buffer_empty_before = ring_buffer_empty(tsk->rcv_buf);
                    // int data_len = cb->pl_len;
                    // int free_space = ring_buffer_free(tsk->rcv_buf);

                    // if (free_space >= data_len)
                    // {
                    //     write_ring_buffer(tsk->rcv_buf, cb->payload, data_len);

                    //     int buf_occupied = ring_buffer_used(tsk->rcv_buf);
                    //     tsk->rcv_wnd = TCP_DEFAULT_WINDOW - buf_occupied;

                    //     if (ring_buffer_empty_before == 1)
                    //         wake_up(tsk->wait_recv);

                    //     tsk->rcv_nxt += data_len;
                    //     log(DEBUG, "server sent ack pack, ack_num is: %u", tsk->rcv_nxt);
                    //     tcp_send_control_packet(tsk, TCP_ACK);
                    //     tcp_move_recv_ofo_buffer(tsk);
                    //     log(DEBUG, "tsk->rcv_nxt after processing data packet: %u", tsk->rcv_nxt);
                    // }
                    // else
                    // {
                    //     log(ERROR, "Receive buffer overflow: required %d, available %d",
                    //         data_len, free_space);
                    //     // 缓冲区空间不足则丢弃数据
                    //     break;
                    // }
                }
                // 3: 乱序数据包
                else if (less_than_32b(tsk->rcv_nxt, cb->seq))
                {
                    log(DEBUG, "tcp_process in TCP_ESTABLISHED state receive out-of-order packet");

                    // 将数据包添加到乱序队列
                    tcp_recv_ofo_buffer_add_packet(tsk, cb);
                    tcp_send_control_packet(tsk, TCP_ACK);
                }
                // 4: 部分重复的数据包
                else if (less_than_32b(cb->seq, tsk->rcv_nxt) && less_than_32b(tsk->rcv_nxt, cb->seq_end))
                {
                    log(DEBUG, "tcp_process in TCP_ESTABLISHED state receive partially overlapping packet");
                    // 直接丢弃数据包
                    tcp_send_control_packet(tsk, TCP_ACK);
                }
            }
            // // 5: 探测包
            // else if (less_than_32b(cb->seq, tsk->rcv_nxt) && cb->pl_len == 0)                
            // {
            //     log(DEBUG, "tcp_process in TCP_ESTABLISHED state receive probe packet");
            //     log(DEBUG, "seq = %u, cb->pl_len = %d", cb->seq - tsk->iss, cb->pl_len);

            //     // 回应探测包
            //     tcp_send_control_packet(tsk, TCP_ACK);
            // }

            if (cb->flags & TCP_FIN)
            {
                log(DEBUG, "tcp_process in TCP_ESTABLISHED state receive FIN");
                log(DEBUG, "rcv_nxt = %u, cb->seq = %u", tsk->rcv_nxt - tsk->iss, cb->seq - tsk->iss);

                // if(tsk->rcv_nxt==cb->seq)
                // {
                    tsk->rcv_nxt = cb->seq + 1;
                    tcp_set_state(tsk, TCP_CLOSE_WAIT);
                    tcp_send_control_packet(tsk, TCP_ACK);
                    
                    // log(DEBUG, "tcp_process in TCP_ESTABLISHED state wake up tsk->wait_recv");
                    wake_up(tsk->wait_recv);
                // }
            }
            break;

        case TCP_CLOSE_WAIT:
            if (cb->flags & TCP_ACK)
            {
                tcp_update_window_safe(tsk, cb);
                tcp_update_send_buffer(tsk, cb->ack);

                tcp_congestion_control(tsk, cb, packet);
            }

            if (packet && cb->pl_len > 0)
            {
                if (is_tcp_seq_valid(tsk, cb))
                {
                    tcp_recv_ofo_buffer_add_packet(tsk, cb);
                    tcp_send_control_packet(tsk, TCP_ACK);
                }
            }
            break;

        case TCP_LAST_ACK:
            if (cb->flags & TCP_ACK)
            {
                tcp_update_window_safe(tsk, cb);
                tcp_update_send_buffer(tsk, cb->ack);
                tcp_update_retrans_timer(tsk);

                tcp_congestion_control(tsk, cb, packet);

                tcp_set_state(tsk, TCP_CLOSED);
                tcp_unset_retrans_timer(tsk);
                tcp_unhash(tsk);
            }
            break;

        case TCP_FIN_WAIT_1:
            if (cb->flags & TCP_ACK)
            {
                tcp_update_window_safe(tsk, cb);
                tcp_update_send_buffer(tsk, cb->ack);
                tcp_update_retrans_timer(tsk);

                tcp_congestion_control(tsk, cb, packet);

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

        case TCP_CLOSING:
            if (cb->flags & TCP_ACK)
            {
                tcp_update_window_safe(tsk, cb);
                tcp_update_send_buffer(tsk, cb->ack);

                tcp_congestion_control(tsk, cb, packet);

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

// TODO 3.3.4
/*
1. 创建recv_ofo_buf_entry
2. 用list_for_each_entry_safe遍历rcv_ofo_buf，将表项插入合适的位置。如果发现了重复数据包，则丢弃当前数据。
3. 调用tcp_move_recv_ofo_buffer执行报文上送

在接收到数据报文时调用。
*/
int tcp_recv_ofo_buffer_add_packet(struct tcp_sock *tsk, struct tcp_cb *cb)
{
    if (cb->pl_len <= 0)
    {
        return 0;
    }

    pthread_mutex_lock(&tsk->rcv_buf_lock);

    struct recv_ofo_buf_entry *entry = malloc(sizeof(struct recv_ofo_buf_entry));
    if (!entry)
    {
        log(ERROR, "malloc recv ofo buffer entry failed");
        pthread_mutex_unlock(&tsk->rcv_buf_lock);
        return -1;
    }

    entry->data = malloc(cb->pl_len);
    if (!entry->data)
    {
        log(ERROR, "malloc data buffer failed");
        free(entry);
        pthread_mutex_unlock(&tsk->rcv_buf_lock);
        return -1;
    }
    memcpy(entry->data, cb->payload, cb->pl_len);
    entry->len = cb->pl_len;
    entry->seq = cb->seq;
    entry->seq_end = cb->seq_end;

    int inserted = 0;
    // 检查是否有重复数据包并找到合适插入位置
    struct recv_ofo_buf_entry *pos, *q;
    list_for_each_entry_safe(pos, q, &tsk->rcv_ofo_buf, list)
    {
        // 检测重复数据包
        if (less_than_32b(entry->seq, pos->seq_end) && less_than_32b(pos->seq, entry->seq_end))
        {
            log(DEBUG, "tcp_recv_ofo_buffer_add_packet: duplicate packet, seq = %u, seq_end = %u", entry->seq - tsk->iss, entry->seq_end - tsk->iss);

            free(entry->data);
            free(entry);
            pthread_mutex_unlock(&tsk->rcv_buf_lock);
            return 0;
        }

        // 找到合适位置的插入
        if (less_than_32b(entry->seq, pos->seq))
        {
            log(DEBUG, "tcp_recv_ofo_buffer_add_packet: insert packet, seq = %u, seq_end = %u", entry->seq - tsk->iss, entry->seq_end - tsk->iss);

            list_insert(&entry->list, pos->list.prev, &pos->list);
            inserted = 1;
            break;
        }
    }

    if (!inserted)
    {
        list_add_tail(&entry->list, &tsk->rcv_ofo_buf);
    }

    pthread_mutex_unlock(&tsk->rcv_buf_lock);

    tcp_move_recv_ofo_buffer(tsk);

    return 0;
}

/*
遍历rcv_ofo_buf，将所有有序的（序列号等于tsk->rcv_nxt）的报文送入接收队列（tsk->rcv_buf）
更新rcv_nxt, rcv_wnd并唤醒接收线程(wait_recv)
如果接收队列已满，应当退出函数，而非等待。

在tcp_recv_ofo_buffer_add_packet执行最后调用。
*/
int tcp_move_recv_ofo_buffer(struct tcp_sock *tsk)
{
    int moved = 0;

    pthread_mutex_lock(&tsk->rcv_buf_lock);

    struct recv_ofo_buf_entry *entry, *q;
    // 遍历所有乱序包，找到序号正确的包(seq == rcv_nxt)
    list_for_each_entry_safe(entry, q, &tsk->rcv_ofo_buf, list)
    {
        log(DEBUG, "tcp_move_recv_ofo_buffer: entry seq = %u, rcv_nxt = %u, next seq = %u", entry->seq - tsk->iss, tsk->rcv_nxt - tsk->iss, q->seq - tsk->iss);

        if (entry->seq == tsk->rcv_nxt)
        {
            // 添加到接收缓冲区
            int free_space = ring_buffer_free(tsk->rcv_buf);
            if (free_space <= 0)
            {
                break;
            }

            log(DEBUG, "tcp_move_recv_ofo_buffer: entry->seq = tck->recv_nxt");

            int to_write = entry->len > free_space ? free_space : entry->len;

            write_ring_buffer(tsk->rcv_buf, entry->data, to_write);

            tsk->rcv_nxt = entry->seq_end;

            list_delete_entry(&entry->list);
            free(entry->data);
            free(entry);

            moved++;
        }
        else if (less_than_32b(entry->seq, tsk->rcv_nxt))
        {
            list_delete_entry(&entry->list);
            free(entry->data);
            free(entry);
        }
        else
        {
            break;
        }
    }

    // 正常接收缓冲区占用的空间
    int buf_occupied = ring_buffer_used(tsk->rcv_buf);

    // 乱序缓冲区占用的空间
    int ofo_occupied = 0;
    list_for_each_entry(entry, &tsk->rcv_ofo_buf, list)
    {
        ofo_occupied += entry->len;
    }

    // 更新接收窗口
    tsk->rcv_wnd = TCP_DEFAULT_WINDOW - buf_occupied - ofo_occupied;

    if (moved > 0)
    {
        wake_up(tsk->wait_recv);
    }

    pthread_mutex_unlock(&tsk->rcv_buf_lock);
    return moved;
}

//TODO 4.1
/*
函数tcp_congestion_control根据当前TCP拥塞控制的阶段(tsk->c_state)和收到的ACK数据包信息(cb->ack和ack_valid),
 更新拥塞窗口cwnd,慢启动阈值ssthresh等参数.
它通过状态机的方式处理不同的拥塞控制阶段: 
    TCP_SLOW_START,
    TCP_CONGESTION_AVOIDANCE,
    TCP_FAST_RECOVERY 
*/
void tcp_congestion_control(struct tcp_sock *tsk, struct tcp_cb *cb, char *packet)
{
    // 保存旧状态以便检测状态变化
    int old_state = tsk->c_state;

    // 只处理ACK包
    if (!(cb->flags & TCP_ACK))
        return;

    // 判断是新的ACK还是重复ACK
    int is_new_ack = greater_than_32b(cb->ack, tsk->snd_una);
    int is_dup_ack = (cb->flags & TCP_ACK) && (cb->ack == tsk->snd_una);

    log(DEBUG, "拥塞控制状态: %s, cwnd: %u, ssthresh: %u",
        tsk->c_state == TCP_SLOW_START ? "慢启动" : (tsk->c_state == TCP_CONGESTION_AVOIDANCE ? "拥塞避免" : "快速恢复"),
        tsk->cwnd, tsk->ssthresh);

    switch (tsk->c_state)
    {
    case TCP_SLOW_START:
        if (is_new_ack)
        {
            // 慢启动阶段，每收到一个ACK，cwnd增加一个MSS
            tsk->cwnd += TCP_MSS;
            log(DEBUG, "慢启动: cwnd增加到 %u", tsk->cwnd);
            log_tcp_event(tsk, "receive ack in TCP_RENO_SLOW_START");

            // 如果cwnd超过阈值，进入拥塞避免状态
            if (tsk->cwnd >= tsk->ssthresh)
            {
                tsk->c_state = TCP_CONGESTION_AVOIDANCE;
                log(DEBUG, "进入拥塞避免状态");
                log_tcp_event(tsk, "receive ack in TCP_RENO_CONGESTION_AVOIDANCE");
            }
        }
        else if (is_dup_ack)
        {
            // 收到重复ACK，计数增加
            tsk->dup_ack_cnt++;

            // 如果收到3个重复ACK，进入快速恢复状态
            if (tsk->dup_ack_cnt >= 3)
            {
                // 快速重传
                tcp_retrans_send_buffer(tsk);

                // 设置新的阈值和cwnd
                tsk->ssthresh = tsk->cwnd / 2;
                tsk->cwnd = tsk->ssthresh + 3 * TCP_MSS;

                tsk->dup_ack_cnt = 0;

                // 设置恢复点
                tsk->recovery_point = tsk->snd_nxt;
                
                tsk->c_state = TCP_FAST_RECOVERY;
                log(DEBUG, "进入快速恢复状态: cwnd=%u, ssthresh=%u", tsk->cwnd, tsk->ssthresh);
                log_tcp_event(tsk, "TCP_CONGESTION into FAST_RECOVERY");
            }
        }
        break;

    case TCP_CONGESTION_AVOIDANCE:
        if (is_new_ack)
        {
            // 拥塞避免阶段，cwnd每个RTT增加1个MSS
            // 近似实现为每收到cwnd个字节确认，cwnd增加1个MSS
            tsk->cwnd += (TCP_MSS * TCP_MSS) / tsk->cwnd;
            log(DEBUG, "拥塞避免: cwnd增加到 %u", tsk->cwnd);
            log_tcp_event(tsk, "receive ack in TCP_RENO_CONGESTION_AVOIDANCE");
        }
        else if (is_dup_ack)
        {
            tsk->dup_ack_cnt++;

            // 3个重复ACK，进入快速恢复
            if (tsk->dup_ack_cnt >= 3)
            {
                tcp_retrans_send_buffer(tsk);

                tsk->ssthresh = tsk->cwnd / 2;
                tsk->cwnd = tsk->ssthresh + 3 * TCP_MSS;
                tsk->dup_ack_cnt = 0;
                tsk->recovery_point = tsk->snd_nxt;

                tsk->c_state = TCP_FAST_RECOVERY;
                log(DEBUG, "进入快速恢复状态: cwnd=%u, ssthresh=%u", tsk->cwnd, tsk->ssthresh);
                log_tcp_event(tsk, "TCP_CONGESTION into FAST_RECOVERY");
            }
        }
        break;

    case TCP_FAST_RECOVERY:
        if (is_new_ack && greater_or_equal_32b(cb->ack, tsk->recovery_point))
        {
            // 收到恢复点后的ACK，退出快速恢复
            tsk->cwnd = tsk->ssthresh;
            tsk->dup_ack_cnt = 0;
            tsk->c_state = TCP_CONGESTION_AVOIDANCE;
            log(DEBUG, "退出快速恢复: cwnd=%u, ssthresh=%u", tsk->cwnd, tsk->ssthresh);
            log_tcp_event(tsk, "cwnd >= recovery_point, change to TCP_RENO_CONGESTION_AVOIDANCE");
        }
        else if (is_new_ack)
        {
            // 收到新ACK但未到恢复点，部分确认
            tsk->cwnd += TCP_MSS;
            log_tcp_event(tsk, "receive ack in TCP_RENO_QUICK_RECOVERY");
        }
        else if (is_dup_ack)
        {
            // 快速恢复阶段收到重复ACK，增加cwnd
            tsk->cwnd += TCP_MSS;
            tsk->dup_ack_cnt++;

            if (tsk->dup_ack_cnt >= 3)
            {
                tcp_retrans_send_buffer(tsk);

                tsk->ssthresh = tsk->cwnd / 2;
                tsk->cwnd = tsk->ssthresh + 3 * TCP_MSS;
                tsk->dup_ack_cnt = 0;
                tsk->recovery_point = tsk->snd_nxt;

                log(DEBUG, "进入快速恢复状态: cwnd=%u, ssthresh=%u", tsk->cwnd, tsk->ssthresh);
                log_tcp_event(tsk, "TCP_CONGESTION into FAST_RECOVERY");
            }
            else
            {
                log_tcp_event(tsk, "receive duplicate ack in TCP_RENO_FAST_RECOVERY");
            }

            log(DEBUG, "快速恢复中收到重复ACK: cwnd=%u", tsk->cwnd);
        }
        break;
    }

    // 记录常规ACK事件
    if (old_state == tsk->c_state && is_new_ack &&
        tsk->c_state != TCP_QUICK_RECOVERY)
    {
        log_tcp_event(tsk, "receive ack in TCP_RENO_");
    }
}

// 事件记录函数
void log_tcp_event(struct tcp_sock *tsk, const char *event_desc)
{
    strncpy(tsk->last_event, event_desc, sizeof(tsk->last_event) - 1);
    tsk->last_event[sizeof(tsk->last_event) - 1] = '\0';
}