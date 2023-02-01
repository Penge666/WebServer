## epoll


主旨思想

- 直接在**内核态**创建` eventpoll实例`(结构体)，通过`epoll`提供的API操作该实例
- 结构体中有`红黑树`和`双链表`，分别用来**存储需要检测的文件描述符**和**存储已经发生改变的文件描述符**

![image-20230129212252494](E:\Lab\cs144\Linux高性能服务器\Note\网络编程\IO多路复用\epoll\ReadMe.assets\image-20230129212252494.png)

**用法**


概览

  ```c
  #include <sys/epoll.h>
  
  // 创建一个新的epoll实例
  // 在内核中创建了一个数据，这个数据中有两个比较重要的数据，一个是需要检测的文件描述符的信息（红黑树），还有一个是就绪列表，存放检测到数据发送改变的文件描述符信息（双向链表）
  int epoll_create(int size);
  
  // 对epoll实例进行管理：添加文件描述符信息，删除信息，修改信息
  int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
  struct epoll_event { 
      uint32_t events; /* Epoll events */ 
      epoll_data_t data; /* User data variable */ 
  };
  typedef union epoll_data { 
      void *ptr; 
      int fd; 
      uint32_t u32; 
      uint64_t u64; 
  } epoll_data_t;
  
  // 检测函数
  int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
  ```

- `int epoll_create(int size);`
  - 功能：创建一个新的epoll实例
  - 参数：`size`，目前没有意义了(之前底层实现是哈希表，现在是红黑树)，随便写一个数，必须大于0
  - 返回值
    - -1：失败
    - \>0：操作`epoll实例`的文件描述符
- `int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);`
  - 功能：对epoll实例进行管理：添加文件描述符信息，删除信息，修改信息
  - 参数：
    - `epfd`：epoll实例对应的文件描述符
    - `op`：要进行什么操作
      - 添加：`EPOLL_CTL_ADD`
      - 删除：`EPOLL_CTL_DEL`
      - 修改：`EPOLL_CTL_MOD`
    - `fd`：要检测的文件描述符 
    - `event`：检测文件描述符什么事情，通过设置`epoll_event.events`，常见操作
      - 读事件：`EPOLLIN`
      - 写事件：`EPOLLOUT `
      - 错误事件：`EPOLLERR`
      - 设置边沿触发：`EPOLLET`（默认水平触发）
  - 返回值：成功0，失败-1
- `int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);`
  - 功能：检测哪些文件描述符发生了改变
  - 参数：
    - `epfd`：epoll实例对应的文件描述符
    - `events`：传出参数，保存了发生了变化的文件描述符的信息
    - `maxevents`：第二个参数结构体数组的大小 
    - `timeout`：阻塞时长 
      - 0：不阻塞
      - -1：阻塞，当检测到需要检测的文件描述符有变化，解除阻塞
      - \>0：具体的阻塞时长(ms)
  - 返回值：
    -  \> 0：成功，返回发送变化的文件描述符的个数
    - -1：失败

**运行结果**

![image-20230129212837561](E:\Lab\cs144\Linux高性能服务器\Note\网络编程\IO多路复用\epoll\ReadMe.assets\image-20230129212837561.png)