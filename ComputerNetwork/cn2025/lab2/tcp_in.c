#include "tcp.h"
#include "tcp_sock.h"
#include "tcp_timer.h"

#include "log.h"
#include "ring_buffer.h"

#include <stdlib.h>
// update the snd_wnd of tcp_sock
//
// if the snd_wnd before updating is zero, notify tcp_sock_send (wait_send)
static inline void tcp_update_window(struct tcp_sock* tsk, struct tcp_cb* cb)
{
    u16 old_snd_wnd = tsk->snd_wnd;
    tsk->snd_wnd = cb->rwnd;
    if (old_snd_wnd == 0)
        wake_up(tsk->wait_send);
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

// check whether the sequence number of the incoming packet is in the receiving
// window
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
                tcp_send_reset(cb);
            }
            break;

        case TCP_SYN_SENT:
            log(DEBUG, "Received packet in SYN_SENT state");
            if (cb->flags & (TCP_SYN | TCP_ACK))
            {
                tsk->rcv_nxt = cb->seq + 1;
                // tsk->snd_una = cb->ack;
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
                tsk->snd_una = cb->ack;
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
            if (cb->flags & TCP_ACK)
            {
                // tcp_update_window_safe(tsk, cb);

                if (less_or_equal_32b(tsk->snd_una, cb->ack) && less_or_equal_32b(cb->ack, tsk->snd_nxt))
                {
                    tsk->snd_una = cb->ack;
                    tcp_unset_retrans_timer(tsk);

                    if (tsk->snd_wnd > 0)
                    {
                        wake_up(tsk->wait_send);
                    }
                }
            }

            if (packet && cb->pl_len > 0)
            {
                if (is_tcp_seq_valid(tsk, cb))
                {
                    int free_space = ring_buffer_free(tsk->rcv_buf);

                    int to_write = cb->pl_len > free_space ? free_space : cb->pl_len;

                    if (to_write > 0)
                    {
                        // log(DEBUG, "cb->payload = %s", cb->payload);

                        write_ring_buffer(tsk->rcv_buf, cb->payload, to_write);
                        tsk->rcv_nxt = cb->seq_end;

                        log(DEBUG, "tcp_process in TCP_ESTABLISHED state wake up tsk->wait_recv when to_write > 0");
                        wake_up(tsk->wait_recv);
                    }

                    if (to_write < cb->pl_len)
                    {
                        log(WARNING, "receive buffer overflow");
                    }

                    tcp_send_control_packet(tsk, TCP_ACK);
                }
                else
                {
                    log(WARNING, "receive packet with invalid sequence number");
                    tcp_send_control_packet(tsk, TCP_ACK);
                }
            }
            // else
            // {
            //     // log(DEBUG, "tcp_process in TCP_ESTABLISHED state no payload");

            //     // wake_up(tsk->wait_recv);
            // }

            if (cb->flags & TCP_FIN)
            {
                tsk->rcv_nxt = cb->seq_end;
                tcp_set_state(tsk, TCP_CLOSE_WAIT);
                tcp_send_control_packet(tsk, TCP_ACK);

                log(DEBUG, "tcp_process in TCP_ESTABLISHED state wake up tsk->wait_recv");
                wake_up(tsk->wait_recv);
            }
            break;

        case TCP_FIN_WAIT_1:
            if (cb->flags & TCP_ACK)
            {
                tsk->snd_una = cb->ack;

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
                tsk->snd_una = cb->ack;
                tcp_set_state(tsk, TCP_CLOSED);
                tcp_unhash(tsk);
            }
            break;

        case TCP_CLOSING:
            if (cb->flags & TCP_ACK)
            {
                tsk->snd_una = cb->ack;
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