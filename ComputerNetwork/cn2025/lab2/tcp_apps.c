#include "tcp_sock.h"

#include "log.h"

#include <unistd.h>
// TODO2.2
// tcp server application, listens to port (specified by arg) and serves only one
// connection request
// 同 tcp_stack_trans.py文件中 server 函数的功能，监听并echo字符串，返回字符串时加上前缀"server echoes:
// "，冒号后面有空格。
void* tcp_server(void* arg)
{
    u16 port = *(u16*)arg;
    struct tcp_sock* tsk = alloc_tcp_sock();

    struct sock_addr addr;
    addr.ip = htonl(0);
    addr.port = port;
    if (tcp_sock_bind(tsk, &addr) < 0)
    {
        log(ERROR, "tcp_sock bind to port %hu failed", ntohs(port));
        exit(1);
    }

    if (tcp_sock_listen(tsk, 3) < 0)
    {
        log(ERROR, "tcp_sock listen failed");
        exit(1);
    }

    log(DEBUG, "listen to port %hu.", ntohs(port));

    struct tcp_sock* csk = tcp_sock_accept(tsk);

    log(DEBUG, "accept a connection.");

    // add my code here
    // int cnt = 0;
    // while (cnt < 10)
    // {
    //     char echo_message[90] = "server echoes: ";
    //     char body[65] = {0};
    //     int read_len = tcp_sock_read(csk, body, 63);

    //     strcat(echo_message, body);
    //     printf("%s\n", echo_message);

    //     cnt++;
    // }
    char body[65];
    int read_len = 0;
    while ((read_len = tcp_sock_read(csk, body, 63)) > 0)
    {
        body[read_len] = '\0';

        char echo_message[90] = "server echoes: ";
        strcat(echo_message, body);

        tcp_sock_write(csk, echo_message, strlen(echo_message));
    }

    sleep(5);

    tcp_sock_close(csk);

    return NULL;
}

// tcp client application, connects to server (ip:port specified by arg), each
// time sends one bulk of data and receives one bulk of data
// 同 tcp_stack_trans.py文件中 client 函数的功能，发送不少于5次的字符串循环。
void* tcp_client(void* arg)
{
    struct sock_addr* skaddr = arg;

    struct tcp_sock* tsk = alloc_tcp_sock();

    if (tcp_sock_connect(tsk, skaddr) < 0)
    {
        log(ERROR,
            "tcp_sock connect to server (" IP_FMT ":%hu)failed.",
            NET_IP_FMT_STR(skaddr->ip),
            ntohs(skaddr->port));
        exit(1);
    }

    // add my code here
    int len = 62;
    char base_str[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char send_str[65];
    char recv_buf[90];
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < len; j++)
        {
            send_str[j] = base_str[((i + j) % len)];
        }
        send_str[len] = '\0';

        tcp_sock_write(tsk, send_str, 63);

        int read_len = tcp_sock_read(tsk, recv_buf, sizeof(recv_buf) - 1);
        if (read_len > 0)
        {
            recv_buf[read_len] = '\0';
            printf("%s\n", recv_buf);
        }
    }

    sleep(1);

    tcp_sock_close(tsk);

    return NULL;
}
