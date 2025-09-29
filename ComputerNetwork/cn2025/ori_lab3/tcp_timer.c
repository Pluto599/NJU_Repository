#include "tcp_timer.h"
#include "tcp.h"
#include "tcp_sock.h"

#include "log.h"

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static struct list_head timer_list;
static pthread_mutex_t timer_list_lock = PTHREAD_MUTEX_INITIALIZER;

// TODO 2.1.4 done
// scan the timer_list, find the tcp sock which stays for at 2*MSL, release it
// TODO 3.2.4
/*
tcp_scan_timer_list中增加对Persist Timer超时的处理。
如果TCP没有关闭，且发送窗口snd_wnd小于TCP_MSS，则发送一个Probe报文、重置时间，否则关闭该定时器。
*/
// TODO 3.3.2
/*
增加对重传计时器的支持。
超时发生时，检查socket未关闭。如果达到重传次数上限（自行设置，比如3），则强制关闭socket（发送RST，用unhash之类的函数将socket从各种绑定的队列中移除，并唤醒wait_connect之类的信号量）。如果未达到重传上限，则调用tcp_retrans_send_buffer执行重传，并更新重传次数和timeout时间。
*/
void tcp_scan_timer_list()
{
    // fprintf(stdout, "TODO: implement %s please.\n", __FUNCTION__);
    struct tcp_sock* tsk;
    struct tcp_timer *timer, *tmp;

    // log(DEBUG,"before timer_list_lock");
    pthread_mutex_lock(&timer_list_lock);
    // log(DEBUG,"after timer_list_lock");

    list_for_each_entry_safe(timer, tmp, &timer_list, list)
    {
        if (timer->enable)
        {
            timer->timeout -= TCP_TIMER_SCAN_INTERVAL;

            switch (timer->type)
            {
                case 0: // timewait
                log(DEBUG, "tcp_scan_timer_list: timewait timer");
                log(DEBUG, "timewait timeout = %d", timer->timeout);
                    if (timer->timeout <= 0)
                    {
                        log(DEBUG, "tcp_scan_timer_list: timewait timer expired");
                        log(DEBUG, "tsk->state = %d", tsk->state);

                        tsk = timewait_to_tcp_sock(timer);

                        tcp_unset_timewait_timer(tsk);

                        if (tsk->state == TCP_TIME_WAIT)
                        {
                            log(DEBUG, "Timewait timer expired, turning to closed");

                            tcp_set_state(tsk, TCP_CLOSED);
                            tcp_unhash(tsk);
                        }
                        free_tcp_sock(tsk);
                    }
                    break;

                case 1: // retrans
                    if (timer->timeout <= 0)
                    {
                        tsk = retranstimer_to_tcp_sock(timer);

                        if (tsk->state != TCP_CLOSED)
                        {
                            // 检查是否达到重传上限（3次）
                            if (timer->retrans_cnt >= 3)
                            {
                                // 达到上限，强制关闭连接
                                log(ERROR, "Retransmission timeout, closing connection");

                                // // 发送RST
                                // struct tcp_cb cb;
                                // cb.saddr = tsk->sk_dip;
                                // cb.daddr = tsk->sk_sip;
                                // cb.sport = tsk->sk_dport;
                                // cb.dport = tsk->sk_sport;
                                // tcp_send_reset(&cb);

                                // 唤醒所有等待线程
                                wake_up(tsk->wait_connect);
                                wake_up(tsk->wait_recv);
                                wake_up(tsk->wait_send);
                                
                                // 禁用定时器
                                tcp_unset_retrans_timer(tsk);

                                // 关闭连接
                                tcp_set_state(tsk, TCP_CLOSED);
                                tcp_unhash(tsk);
                            }
                            else
                            {
                                // 未达到上限，执行重传
                                log(DEBUG, "Retransmission timer expired, retransmitting");
                                tcp_retrans_send_buffer(tsk);

                                // 更新计数和超时时间
                                timer->retrans_cnt++;
                                timer->timeout = TCP_RETRANS_INTERVAL_INITIAL * (1 << timer->retrans_cnt);
                            }
                        }
                        else
                        {
                            // Socket已关闭，禁用定时器
                            tcp_unset_retrans_timer(tsk);
                        }
                    }
                    break;

                case 2: // persist
                    if (timer->timeout <= 0)
                    {
                        tsk = persistimer_to_tcp_sock(timer);
                        if(!tcp_tx_window_test(tsk))
                        {
                            log(DEBUG, "snd_wnd filled, persist timer active");
                            tcp_send_probe_packet(tsk);
                            timer->timeout = TCP_RETRANS_INTERVAL_INITIAL;
                        }
                        else
                        {
                            log(DEBUG, "snd_wnd not filled, persist timer inactive");
                            tcp_unset_persist_timer(tsk);
                        }
                    }
                    break;
            }      
        }
    }

    // log(DEBUG,"before timer_list_lock unlock");
    pthread_mutex_unlock(&timer_list_lock);
    // log(DEBUG,"after timer_list_lock unlock");
}

// set the timewait timer of a tcp sock, by adding the timer into timer_list
void tcp_set_timewait_timer(struct tcp_sock* tsk)
{
    // fprintf(stdout, "TODO: implement %s please.\n", __FUNCTION__);
    // log(DEBUG, "before timer_list_lock");
    pthread_mutex_lock(&timer_list_lock);
    // log(DEBUG, "after timer_list_lock");

    if(!tsk->timewait.enable)
    {
        tsk->timewait.type = 0;
        tsk->timewait.timeout = TCP_TIMEWAIT_TIMEOUT;
        tsk->timewait.enable = 1;
        list_add_tail(&tsk->timewait.list, &timer_list);
    }

    // log(DEBUG, "before timer_list_lock unlock");
    pthread_mutex_unlock(&timer_list_lock);
    // log(DEBUG, "after timer_list_lock unlock");
}

void tcp_unset_timewait_timer(struct tcp_sock* tsk)
{
    // log(DEBUG, "before timer_list_lock");
    pthread_mutex_lock(&timer_list_lock);
    // log(DEBUG, "after timer_list_lock");

    if(tsk->timewait.enable)
    {
        tsk->timewait.enable = 0;

        list_delete_entry(&tsk->timewait.list);
        init_list_head(&tsk->timewait.list);
        free_tcp_sock(tsk);
    }

    // log(DEBUG, "before timer_list_lock unlock");
    pthread_mutex_unlock(&timer_list_lock);
    // log(DEBUG, "after timer_list_lock unlock");
}

// TODO 3.3.2
/*
1. 如果已经启用，则更新超时时间为当前的RTO后退出
2. 创建定时器，设置各个成员变量，初始RTO为TCP_RETRANS_INTERVAL_INITIAL。
3. 增加tsk的引用计数，将定时器加入timer_list末尾

和tcp_send_buffer_add_packet绑定。在tcp_send_packet发送数据报文，以及tcp_send_control_packet发送SYN、FIN报文时调用。TCP协议中ACK报文是不需要重传的。
*/
void tcp_set_retrans_timer(struct tcp_sock* tsk)
{
    // log(DEBUG, "before timer_list_lock");
    pthread_mutex_lock(&timer_list_lock);
    // log(DEBUG, "after timer_list_lock");

    if (tsk->retrans_timer.enable)
    {
        tsk->retrans_timer.timeout = TCP_RETRANS_INTERVAL_INITIAL;
    }
    else
    {
        tsk->retrans_timer.type = 1;
        tsk->retrans_timer.timeout = TCP_RETRANS_INTERVAL_INITIAL;
        tsk->retrans_timer.enable = 1;
        tsk->retrans_timer.retrans_cnt = 0;

        list_add_tail(&tsk->retrans_timer.list, &timer_list);
        tsk->ref_cnt++;
    }

    // log(DEBUG, "before timer_list_lock unlock");
    pthread_mutex_unlock(&timer_list_lock);
    // log(DEBUG, "after timer_list_lock unlock");
}

/*
1. 如果已经禁用，不做任何事
2. 调用free_tcp_sock减少tsk引用计数，并从链表中移除timer

在连接建立成功时，即进入TCP_ESTABLISHED时。关闭连接类似，和上面的设置定时器对应。
*/
void tcp_unset_retrans_timer(struct tcp_sock* tsk)
{
    // log(DEBUG, "before timer_list_lock");
    pthread_mutex_lock(&timer_list_lock);
    // log(DEBUG, "after timer_list_lock");

    if (tsk->retrans_timer.enable)
    {
        tsk->retrans_timer.enable = 0;

        list_delete_entry(&tsk->retrans_timer.list);
        init_list_head(&tsk->retrans_timer.list);
        free_tcp_sock(tsk);
    }

    // log(DEBUG, "before timer_list_lock unlock");
    pthread_mutex_unlock(&timer_list_lock);
    // log(DEBUG, "after timer_list_lock unlock");
}

/*
1. 确认定时器是启用状态
2. 如果发送队列为空，则删除定时器，并且唤醒发送数据的进程。否则重置计时器，包括timeout和重传计数。
注意调用这个函数之前，需要完成对发送队列的更新。

数据传输阶段收到ACK时调用。
*/
void tcp_update_retrans_timer(struct tcp_sock *tsk)
{
    // log(DEBUG, "before timer_list_lock");
    pthread_mutex_lock(&timer_list_lock);
    // log(DEBUG, "after timer_list_lock");

    if (tsk->retrans_timer.enable)
    {
        if (list_empty(&tsk->send_buf))
        {
            tsk->retrans_timer.enable = 0;
            list_delete_entry(&tsk->retrans_timer.list);
            init_list_head(&tsk->retrans_timer.list);

            wake_up(tsk->wait_send); 
        }
        else
        {
            // 重置定时器状态
            tsk->retrans_timer.timeout = TCP_RETRANS_INTERVAL_INITIAL;
            tsk->retrans_timer.retrans_cnt = 0;
        }
    }

    // log(DEBUG, "before timer_list_lock unlock");
    pthread_mutex_unlock(&timer_list_lock);
    // log(DEBUG, "after timer_list_lock unlock");
}


// scan the timer_list periodically by calling tcp_scan_timer_list
void* tcp_timer_thread(void* arg)
{
    init_list_head(&timer_list);
    while (1)
    {
        usleep(TCP_TIMER_SCAN_INTERVAL);
        tcp_scan_timer_list();
    }

    return NULL;
}


// TODO 3.2.1
// 启用persist timer
/*
1. 如果已经启用，则直接退出
2. 创建定时器，设置各个成员变量，设置timeout为比如TCP_RETRANS_INTERVAL_INITIAL
3. 增加tsk的引用计数，将定时器加入timer_list末尾
*/
void tcp_set_persist_timer(struct tcp_sock *tsk) 
{
    // log(DEBUG, "before timer_list_lock");
    pthread_mutex_lock(&timer_list_lock);
    // log(DEBUG, "after timer_list_lock");
    
    if (!tsk->persist_timer.enable)
    {
        tsk->persist_timer.type = 2;
        tsk->persist_timer.timeout = TCP_RETRANS_INTERVAL_INITIAL;
        tsk->persist_timer.enable = 1;
        list_add_tail(&tsk->persist_timer.list, &timer_list);

        tsk->ref_cnt++;
    }

    // log(DEBUG, "before timer_list_lock unlock");
    pthread_mutex_unlock(&timer_list_lock);
    // log(DEBUG, "after timer_list_lock unlock");
}

// TODO 3.2.2
// 禁用persist timer
/*
1. 如果已经禁用，不做任何事
2. 调用free_tcp_sock减少tsk引用计数，并从链表中移除timer
*/
void tcp_unset_persist_timer(struct tcp_sock *tsk) 
{
    // log(DEBUG, "before timer_list_lock");
    pthread_mutex_lock(&timer_list_lock);
    // log(DEBUG, "after timer_list_lock");

    if (tsk->persist_timer.enable)
    {
        tsk->persist_timer.enable = 0;
        list_delete_entry(&tsk->persist_timer.list);
        free_tcp_sock(tsk);
    }

    // log(DEBUG, "before timer_list_lock unlock");
    pthread_mutex_unlock(&timer_list_lock);
    // log(DEBUG, "after timer_list_lock unlock");
}
