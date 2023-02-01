#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include "locker.h"
#include <sys/uio.h>

class http_conn{

public:

    static int m_epollfd;       // 所有socket上的事件都被注册到同一个epoll内核事件中，所以设置成静态的
    static int m_user_count;    // 统计用户的数量


    //构造函数
    http_conn(){}
    //析构函数
    ~http_conn(){}

    void process(); // 处理客户端请求
    void init(int sockfd, const sockaddr_in& addr);//初始化新接受的连接
    void close_conn();  // 关闭连接
    bool read();// 非阻塞读
    bool write();// 非阻塞写

private:
    int m_sockfd;             // 该HTTP连接的socket和对方的socket地址
    sockaddr_in m_address;

};

#endif
