# Linux高并发服务器项目

## WebServer

基于Linux系统下高性能WEB服务器 ~

## 开发环境

VMWare + ubuntu18.04 + gcc version 4.8.5 

## 技能点

- 熟悉Linux操作系统，了解虚拟地址空间、静态库/动态库及常见API。
- 熟练使用多进程和多线程的相关原理和实战编程。
- 熟练掌握Socket网络编程，熟悉TCP/IP协议栈，如TCP协议、UDP协议。
- 熟悉select、poll、epoll等多路lO复用模型。
- 熟悉Linux上的五种lO模型，阻塞、非阻塞、IO复用、信号驱动、异步。
- 熟悉两种常用事务处理：Reactor模式、Proactor模式。

## 功能

- 利用IO复用技术Epoll与线程池实现多线程的仿Reactor高并发模型。
- 利用状态机解析HTTP请求报文，实现处理静态资源的请求。
- 基于双向链表O（n）实现的定时器，关闭超时的非活动连接。
- 使用Webbench进行压力测试。

## 目录树

```bash
E:.
├─Note
│  ├─多线程
│  ├─多进程
│  │  ├─exec函数族详解.assets
│  │  ├─fork函数详解.assets
│  │  ├─wait和waitpid函数.assets
│  │  ├─信号.assets
│  │  ├─共享内存.assets
│  │  ├─内存映射.assets
│  │  ├─守护进程.assets
│  │  └─管道(匿名,有名).assets
│  ├─网络编程
│  │  ├─IO多路复用
│  │  │  ├─epoll
│  │  │  │  └─ReadMe.assets
│  │  │  ├─poll
│  │  │  │  └─ReadMe.assets
│  │  │  └─select
│  │  │      └─ReadMe.assets
│  │  ├─TCP通信
│  │  │  └─ReadMe.assets
│  │  ├─TCP通信并发
│  │  │  ├─多线程实现TCP并发通信
│  │  │  │  └─ReadMe.assets
│  │  │  └─多进程实现TCP并发通信
│  │  │      └─ReadMe.assets
│  │  └─UDP通信
│  │      └─ReadMe.assets
│  └─项目实战
│      └─未加入业务代码
└─Project
    ├─doc
    ├─noactive
    ├─ReadMe.assets
    ├─resources
    │  └─images
    └─test_presure
        └─webbench-1.5
            └─debian
```



感谢：

[Linux高并发服务器项目](https://www.nowcoder.com/courses/cover/live/504)

[linuxServer](https://github.com/cv-programmer/linuxServer)