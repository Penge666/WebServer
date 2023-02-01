# Poll

 **主旨思想**

- 用一个结构体记录文件描述符集合，并记录用户态状态和内核态状态


 ```c++
  #include <poll.h> 
  struct pollfd { 
      int fd; /* 委托内核检测的文件描述符 */ 
      short events; /* 委托内核检测文件描述符的什么事件 */ 
      short revents; /* 文件描述符实际发生的事件 */ 
  };
  
  int poll(struct pollfd *fds, nfds_t nfds, int timeout);
 ```

- `int poll(struct pollfd *fds, nfds_t nfds, int timeout); `

  - 通过`man poll`查看帮助
  - 参数
    - `fds`：是一个`struct pollfd` 结构体数组，这是一个需要检测的文件描述符的集合
    - `nfds`：这个是第一个参数数组中最后一个有效元素的下标 + 1 
    - `timeout`：阻塞时长 
      - 0：不阻塞
      - -1：阻塞，当检测到需要检测的文件描述符有变化，解除阻塞
      - \>0：具体的阻塞时长(ms)
  - 返回值
    - -1：失败
    - \>0(n)：检测的集合中有n个文件描述符发生了变化


- `events`及`revents`取值，如果有多个事件需要检测，用`|`即可，如同时检测读和写：`POLLIN | POLLOUT`

![image-20230129211827947](E:\Lab\cs144\Linux高性能服务器\Note\网络编程\IO多路复用\poll\ReadMe.assets\image-20230129211827947.png)


**运行截图**

![image-20230129211650500](E:\Lab\cs144\Linux高性能服务器\Note\网络编程\IO多路复用\poll\ReadMe.assets\image-20230129211650500.png)