#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>

#define SERVERIP "127.0.0.1"
#define PORT 6786


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
    
    struct pollfd fds[1024];
    // 初始化
    for (int i = 0; i < 1024; i++) {
        fds[i].fd = -1;
        fds[i].events = POLLIN;
    }
    // 将监听文件描述符加入
    fds[0].fd = listenfd;
    int nfds = 0;
    // 不断循环等待客户端连接
    while (1) {
        // 使用poll，设置为永久阻塞，有文件描述符变化才返回
        int num = poll(fds, nfds + 1, -1);
        if (num == -1) {
            perror("poll");
            exit(-1);
        } else if (num == 0) {
            // 当前无文件描述符有变化，执行下一次遍历
            // 在本次设置中无效（因为select被设置为永久阻塞）
            continue;
        } else {
            // 首先判断监听文件描述符是否发生改变（即是否有客户端连接）
            if (fds[0].revents & POLLIN) {
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
                // 遍历集合, 将新的需要监听的文件描述符加入集合
                for (int i = 1; i < 1024; i++) {
                    if (fds[i].fd == -1) {
                        fds[i].fd = connfd;
                        fds[i].events = POLLIN;
                        break;
                    }
                }
                // 更新最大的监听文件描述符集合下标
                // 存在问题：使用文件描述符替代最大对应下标
                nfds = nfds > connfd ? nfds : connfd;
            }

            // 遍历集合判断是否有变动，如果有变动，那么通信
            char recv_buf[1024] = {0};
            for (int i = 1; i <= nfds; i++) {
                if (fds[i].fd != -1 && fds[i].revents & POLLIN) {
                    ret = read(fds[i].fd, recv_buf, sizeof(recv_buf));
                    if (ret == -1) {
                        perror("read");
                        exit(-1);
                    } else if (ret > 0) {
                        printf("recv server data : %s\n", recv_buf);
                        write(fds[i].fd, recv_buf, strlen(recv_buf));
                    } else {
                        // 表示客户端断开连接
                        printf("client closed...\n");
                        close(fds[i].fd);
                        fds[i].fd = -1;
                        break;
                    }
                }
            }
        }
    }

    close(listenfd);
    return 0;
}