# select

**主旨思想**

1. 首先要构造一个关于文件描述符的列表，将要监听的文件描述符添加到该列表中
2. 调用一个系统函数(`select`)，监听该列表中的文件描述符，直到这些描述符中的一个或者多个进行I/O操作时，该函数才返回
   - 这个函数是阻塞
   - 函数对文件描述符的检测的操作是由内核完成的
3. 在返回时，它会告诉进程有多少（哪些）描述符要进行I/O操作

**用法**

  ```c++
  #include <sys/time.h> 
  #include <sys/types.h> 
  #include <unistd.h> 
  #include <sys/select.h> 
  
  int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
  
  // 将参数文件描述符fd对应的标志位设置为0 
  void FD_CLR(int fd, fd_set *set); 
  // 判断fd对应的标志位是0还是1， 返回值 ： fd对应的标志位的值，0，返回0， 1，返回1 
  int FD_ISSET(int fd, fd_set *set); 
  // 将参数文件描述符fd 对应的标志位，设置为1 
  void FD_SET(int fd, fd_set *set);
  // fd_set一共有1024 bit, 全部初始化为0 
  void FD_ZERO(fd_set *set);
  ```

- `int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout); `

  - 通过`man select`查看帮助
  - 参数
    - `nfds`：委托内核检测的最大文件描述符的值 + 1（+1是因为遍历是下标从0开始，for循环＜设定）
    - `readfds`：要检测的文件描述符的读的集合，委托内核检测哪些文件描述符的读的属性 
      - 一般检测读操作 
      - 对应的是对方发送过来的数据，因为读是被动的接收数据，检测的就是读缓冲区
      - 是一个传入传出参数
    - `writefds`：要检测的文件描述符的写的集合，委托内核检测哪些文件描述符的写的属性 
      - 委托内核检测写缓冲区是不是还可以写数据（不满的就可以写）
    - `exceptfds`：检测发生异常的文件描述符的集合，一般不用
    - `timeout`：设置的超时时间，含义见**`select`参数列表说明**
      - `NULL`：永久阻塞，直到检测到了文件描述符有变化 
      - `tv_sec = tv_usec = 0`， 不阻塞
      - ` tv_sec > 0,tv_usec > 0`：阻塞对应的时间 

  - 返回值
    - -1：失败
    - \>0(n)：检测的集合中有n个文件描述符发生了变化  

- `select`参数列表说明

  - `fd_set`：是一块固定大小的缓冲区(结构体)，`sizeof(fd_set)=128`，即对应1024个比特位

  - `timeval `：结构体类型

    ```c++
    struct timeval { 
        long tv_sec; /* seconds */ 
        long tv_usec; /* microseconds */ 
    };
    ```

**运行截图**

![image-20230129211019595](E:\Lab\cs144\Linux高性能服务器\Note\网络编程\IO多路复用\select\ReadMe.assets\image-20230129211019595.png)