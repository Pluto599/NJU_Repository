#include "tcp_sock.h"
#include "log.h"

#include <unistd.h>
#include <stdio.h>
// TODO 3.1.5
/*
修改tcp_apps.c，使之能够收发文件。具体的逻辑是：Client将client-input.dat中的内容传输给Server，Server将收到的内容存⼊server-output.dat中。

提供的bulk.py中实现了python版的文件传输功能，C程序并不需要实现的完全一样，能进行文件传输，最终保存正确输出文件即可。
*/

// tcp server application, listens to port (specified by arg) and serves only one connection request
// TODO 2.2.2 done
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

    // lab3
    FILE *fp = fopen("server-output.dat", "w");
    if (!fp)
    {
        log(ERROR, "cannot open server-output.dat for writing");
        exit(1);
    }

    char buffer[1000];
    int read_len = 0;
    while (1)
    {
        read_len = tcp_sock_read(csk, buffer, sizeof(buffer));

        if(read_len <= 0)
        {
            log(DEBUG, "Connection closed by peer or error occurred");
            break; // 连接已关闭或发生错误
        }
        
        // log(DEBUG, "Received data length: %d", read_len);

        fwrite(buffer, 1, read_len, fp);
    }

    fclose(fp);

    sleep(5);

    tcp_sock_close(csk);

    return NULL;
}

// tcp client application, connects to server (ip:port specified by arg), each time sends one bulk of data and receives one bulk of data
// TODO 2.2.2 done
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

    FILE *fp = fopen("client-input.dat", "r");
    if (!fp)
    {
        log(ERROR, "cannot open client-input.dat for reading");
        exit(1);
    }

    char buffer[1000];
    int read_bytes = 0;
    while ((read_bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0)
    {
        int ret = tcp_sock_write(tsk, buffer, read_bytes);

        if(ret < 0)
        {
            log(ERROR, "Failed to send data to server.");
            break; // 发送失败，退出循环
        }            

        // usleep(1000); // sleep for 1ms to simulate network delay
    }

    fclose(fp);

    sleep(1);

    tcp_sock_close(tsk);

    return NULL;
}
