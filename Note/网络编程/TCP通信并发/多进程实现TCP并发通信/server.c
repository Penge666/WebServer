#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#define SERVERIP "127.0.0.1"
#define PORT 6788

void recycleChild(int arg){
    //while处理多个信号
    while(1){
        //参数设置WNOHANG位，与那么该系统调用就是非阻塞的，也就是说会立刻返回而不是等待子进程的状态发生变化。
        int ret = waitpid(-1,NULL,WNOHANG);
        if(ret == -1){
            // 所有子进程都回收了
            break;
        }else if(ret==0){
            // 还有子进程活着
            break;
        }else{
            // 回收子进程
            printf("子进程 %d 被回收了\n", ret);
        }
    }
}
int main(){
    //0. 注册信号捕捉
    struct sigaction act;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    act.sa_handler = recycleChild;
    sigaction(SIGCHLD, &act, NULL);

    //1. 创建socket（用于监听的套接字）
    // AF_INET ：ipv4   SOCK_STREAM : 流式协议(TCP) 
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd==-1){
        perror("socket");
        exit(-1);
    }
    // 2. 绑定，将fd 和本地的IP和端口进行绑定
    struct sockaddr_in server_addr; //TCP/IP协议族
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

    // 3. 监听这个socket上的连接

    ret = listen(listenfd, 8);//第2个参数：未连接的和已经连接的和的最大值
    if (ret == -1) {
        perror("listen");
        exit(-1);
    }

// 不断循环等待客户端连接
    while (1) {
        // 4. 接收客户端连接
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int connfd = accept(listenfd, (struct sockaddr*)&client_addr, &client_addr_len);
        if (connfd == -1) {
            // 用于处理信号捕捉导致的accept: Interrupted system call
            if (errno == EINTR) {
                continue;
            }
            perror("accept");
            exit(-1);
        }
        pid_t pid = fork();
        if (pid == 0) {
            // 子进程
            // 输出客户端信息，IP组成至少16个字符（包含结束符）
            char client_ip[16] = {0};
            inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_ip, sizeof(client_ip));
            unsigned short client_port = ntohs(client_addr.sin_port);
            printf("ip:%s, port:%d\n", client_ip, client_port);

            // 5. 开始通信
            // 服务端先接收客户端信息，再向客户端发送数据
            // 接收数据
            char recv_buf[1024] = {0};
            while (1) {
                ret = read(connfd, recv_buf, sizeof(recv_buf));
                if (ret == -1) {
                    perror("read");
                    exit(-1);
                } else if (ret > 0) {
                    printf("recv client data : %s\n", recv_buf);
                } else {
                    // 表示客户端断开连接
                    printf("client closed...\n");
                    // 退出循环，用来解决出现两次client closed...
                    break;
                }
                // 发送数据
                char *send_buf = "hello, i am server";
                // 粗心写成sizeof，那么就会导致遇到空格终止
                write(connfd, send_buf, strlen(send_buf));
            }
            // 关闭文件描述符
            close(connfd);
        }
    }

   


 
    close(listenfd);
    return 0;
}