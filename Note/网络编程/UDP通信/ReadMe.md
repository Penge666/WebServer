# UDP通信

**用法**



![image-20230129221152209](E:\Lab\cs144\Linux高性能服务器\Note\网络编程\UDP通信\ReadMe.assets\image-20230129221152209.png#pic_center)


消息收发函数

- `ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);`
  - 功能：udp发送消息函数
  - 参数
    - `sockfd`：通信的套接字(文件描述符)
    - `buf`：要发送的数据 
    - `len`：发送数据的长度 
    - `flags`：设置为0即可
    - `dest_addr`：通信的另外一端的地址信息 
    - `addrlen`：地址的内存大小，即`sizeof(dest_addr)`
  - 返回值：失败-1，否则返回发送数据大小
- `ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen); `
  - 功能：udp接收消息函数
  - 参数
    - `sockfd`：通信的套接字(文件描述符)
    - `buf`：接收的数据 
    - `len`：接收数据的长度 
    - `flags`：设置为0即可
    - `dest_addr`：通信的另外一端的地址信息，不需要设置为NULL即可
    - `addrlen`：地址的内存大小，即`sizeof(dest_addr)`
  - 返回值：失败-1，否则返回发送数据大小



**运行结果**

![image-20230129221502573](E:\Lab\cs144\Linux高性能服务器\Note\网络编程\UDP通信\ReadMe.assets\image-20230129221502573.png)

【注意】

1. 服务端不需要设置监听文件描述符

2. 不需要多进程/多线程，或者IO多路复用即可实现多并发

#### 