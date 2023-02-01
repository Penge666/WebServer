**思路**

- 服务端使用一个父进程，多个子进程
  - 父进程负责等待并接受客户端的连接
  - 子进程：完成通信，接受一个客户端连接，就创建一个子进程用于通信
- 客户端不需要改变（同一对一通信）

**遇到问题及解决**

- 断开连接后，服务器端如何处理子进程，回收资源？
  - 使用信号处理
- 使用信号捕捉回收子进程资源后，出现服务端`accept: Interrupted system call`，且不能有新客户端连接，如何解决？
  - 产生`EINTR`信号，具体说明通过`man 2 accept`查看
  - 在`accept`返回值处进行判断处理，不输出错误即可
- 当停止所有的客户端连接后，出现`read: Connection reset by peer`，如何解决？
  - 产生的原因：连接断开后的读和写操作引起的
  - 简单修改：将客户端中休眠语句的位置进行更改
  - 方法：[[261]Connection reset by peer的常见原因及解决办法](https://blog.csdn.net/xc_zhou/article/details/80950753)
- 解决上一个问题后，服务端出现两次`client closed...`，如何解决？
  - 是因为在关闭连接后，应该退出循环，所以在该`printf`语句后，添加`break`即可

参考：[第4章 Linux 网络编程](https://github.com/cv-programmer/linuxServer/blob/master/doc/04Linux%E7%BD%91%E7%BB%9C%E7%BC%96%E7%A8%8B.md)

**运行结果**

![image-20230129173619352](E:\Lab\cs144\Linux高性能服务器\Note\网络编程\TCP通信并发\多进程实现TCP并发通信\ReadMe.assets\image-20230129173619352.png)