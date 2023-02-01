#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>

#define SERVERIP "127.0.0.1"
#define PORT 6799


int main()
{
    // 1. 创建socket（用于监听的套接字）
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        perror("socket");
        exit(-1);
    }
    // 2. 绑定
    struct sockaddr_in server_addr;
    server_addr.sin_family = PF_INET;
    // 点分十进制转换为网络字节序
    inet_pton(AF_INET, SERVERIP, &server_addr.sin_addr.s_addr);
    // 服务端也可以绑定0.0.0.0即任意地址
    // server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    int ret = bind(listenfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret == -1) {
        perror("bind");
        exit(-1);
    }
    // 3. 监听
    ret = listen(listenfd, 8);
    if (ret == -1) {
        perror("listen");
        exit(-1);
    }
    
    // 创建epoll实例
    int epfd = epoll_create(100);
    // 将监听文件描述符加入实例
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = listenfd;
    //对epoll实例进行管理：添加文件描述符信息，删除信息，修改信息
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &event);
    if (ret == -1) {
        perror("epoll_ctl");
        exit(-1);
    }
    // 此结构体用来保存内核态返回给用户态发生改变的文件描述符信息
    struct epoll_event events[1024];
    // 不断循环等待客户端连接
    while (1) {
        // 使用epoll，设置为永久阻塞，有文件描述符变化才返回
        // epoll_wait()：检测哪些文件描述符发生了改变
        int num = epoll_wait(epfd, events, 1024, -1);
        if (num == -1) {
            perror("poll");
            exit(-1);
        } else if (num == 0) {
            // 当前无文件描述符有变化，执行下一次遍历
            // 在本次设置中无效（因为select被设置为永久阻塞）
            continue;
        } else {
            // 遍历发生改变的文件描述符集合
            for (int i = 0; i < num; i++) {
                // 判断监听文件描述符是否发生改变（即是否有客户端连接）
                int curfd = events[i].data.fd;
                if (curfd == listenfd) {
                    // 4. 接收客户端连接
                    struct sockaddr_in client_addr;
                    socklen_t client_addr_len = sizeof(client_addr);
                    int connfd = accept(listenfd, (struct sockaddr*)&client_addr, &client_addr_len);
                    if (connfd == -1) {
                        perror("accept");
                        exit(-1);
                    }
                    // 输出客户端信息，IP组成至少16个字符（包含结束符）
                    char client_ip[16] = {0};
                    inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_ip, sizeof(client_ip));
                    unsigned short client_port = ntohs(client_addr.sin_port);
                    printf("ip:%s, port:%d\n", client_ip, client_port);
                    // 将信息加入监听集合
                    event.events = EPOLLIN;
                    event.data.fd = connfd;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &event);
                } else {
                    // 只检测读事件
                    if (events[i].events & EPOLLOUT) {
                        continue;
                    }
                    // 接收消息
                    char recv_buf[1024] = {0};
                    ret = read(curfd, recv_buf, sizeof(recv_buf));
                    if (ret == -1) {
                        perror("read");
                        exit(-1);
                    } else if (ret > 0) {
                        printf("recv server data : %s\n", recv_buf);
                        write(curfd, recv_buf, strlen(recv_buf));
                    } else {
                        // 表示客户端断开连接
                        printf("client closed...\n");
                        close(curfd);
                        epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, NULL);
                        break;
                    }
                }
            }
        }
    }

    close(listenfd);
    close(epfd);
    return 0;
}