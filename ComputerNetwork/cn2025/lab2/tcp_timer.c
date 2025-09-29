#include "tcp_timer.h"
#include "tcp.h"
#include "tcp_sock.h"

#include "log.h"

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static struct list_head timer_list;
static pthread_mutex_t timer_list_lock = PTHREAD_MUTEX_INITIALIZER;

// TODO1.4
// scan the timer_list, find the tcp sock which stays for at 2*MSL, release it
void tcp_scan_timer_list()
{
    // fprintf(stdout, "TODO: implement %s please.\n", __FUNCTION__);
    struct tcp_sock* tsk;
    struct tcp_timer *timer, *tmp;

    pthread_mutex_lock(&timer_list_lock);

    list_for_each_entry_safe(timer, tmp, &timer_list, list)
    {
        if (timer->enable)
        {
            timer->timeout += TCP_TIMER_SCAN_INTERVAL;

            if (timer->timeout >= TCP_TIMEWAIT_TIMEOUT)
            {
                tsk = timewait_to_tcp_sock(timer);
                timer->enable = 0;
                list_delete_entry(&timer->list);

                if (tsk->state == TCP_TIME_WAIT)
                {
                    tcp_set_state(tsk, TCP_CLOSED);
                    tcp_unhash(tsk);
                }
                free_tcp_sock(tsk);
            }
        }
    }

    pthread_mutex_unlock(&timer_list_lock);
}

// set the timewait timer of a tcp sock, by adding the timer into timer_list
void tcp_set_timewait_timer(struct tcp_sock* tsk)
{
    // fprintf(stdout, "TODO: implement %s please.\n", __FUNCTION__);
    pthread_mutex_lock(&timer_list_lock);

    tsk->timewait.type = 0;
    tsk->timewait.timeout = 0;
    tsk->timewait.enable = 1;
    list_add_tail(&tsk->timewait.list, &timer_list);

    pthread_mutex_unlock(&timer_list_lock);
}

// 设置TCP重传定时器
void tcp_set_retrans_timer(struct tcp_sock* tsk)
{
    pthread_mutex_lock(&timer_list_lock);

    tsk->retrans_timer.type = 1;
    tsk->retrans_timer.timeout = 0;

    if (!tsk->retrans_timer.enable)
    {
        tsk->retrans_timer.enable = 1;
        list_add_tail(&tsk->retrans_timer.list, &timer_list);
    }

    pthread_mutex_unlock(&timer_list_lock);
}

// 取消TCP重传定时器
void tcp_unset_retrans_timer(struct tcp_sock* tsk)
{
    pthread_mutex_lock(&timer_list_lock);

    if (tsk->retrans_timer.enable)
    {
        tsk->retrans_timer.enable = 0;
        list_delete_entry(&tsk->retrans_timer.list);
        init_list_head(&tsk->retrans_timer.list);
    }

    pthread_mutex_unlock(&timer_list_lock);
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
