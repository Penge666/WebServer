@[TOC]
# fork函数
fork系统调用用于创建一个新进程，称为子进程，它与进程（称为系统调用fork的进程）同时运行，此进程称为父进程。创建新的子进程后，两个进程将执行fork（）系统调用之后的下一条指令。

通过`man`命令简单查看一下：`man fork`

![在这里插入图片描述](https://img-blog.csdnimg.cn/57cf8f03a5da4e208868b9597c9a3ed3.png)

**基本概念**
- 每个进程都由进程号来标识，其类型为 pid_t（整型），进程号的范围：0～32767。进程号总是唯一的，但可以重用。当一个进程终止后，其进程号就可以再次使用。
- **任何进程（除 init 进程）都是由另一个进程创建**，该进程称为被创建进程的父进程，对应的进程号称为父进程号（PPID）。
- **进程组是一个或多个进程的集合**。他们之间相互关联，进程组可以接收同一终端的各种信号，关联的进程有一个进程组号（PGID）。默认情况下，当前的进程号会当做当前的进程组号

- 进程号和进程组相关函数
  - pid_t getpid(void);：获取进程ID
  - pid_t getppid(void);：获取进程的父进程ID
  - pid_t getpgid(pid_t pid);：获取进程的组ID


`fork()`函数用法
```c
pid_t fork(void);
    函数的作用：用于创建子进程。
    包含头文件：
    	#include <sys/types.h>
		#include <unistd.h>
    返回值：
        fork()的返回值会返回两次。一次是在父进程中，一次是在子进程中。
        在父进程中返回创建的子进程的ID,
        在子进程中返回0
        在父进程中返回-1，表示创建子进程失败，并且设置errno
```
# 例子详解

先来看一个最简单的例子

```c
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
int main() 
{

    // 创建子进程
    pid_t pid = fork();

    // 判断是父进程还是子进程
    if(pid > 0) {
        printf("pid : %d\n", pid);
        // 如果大于0，返回的是创建的子进程的进程号，当前是父进程
        printf("i am parent process, pid : %d, ppid : %d\n", getpid(), getppid());
    } else if(pid == 0) {
        // 当前是子进程
        printf("i am child process, pid : %d, ppid : %d\n", getpid(),getppid());

    }
    for(int i = 0; i < 3; i++) {
        printf("i : %d , pid : %d\n", i , getpid());
        sleep(1);
    }
    return 0;
}
```

输出结果

```bash
(base) user@ubuntu:~/Desktop/OS/NiuKe$ gcc -o test test.c  -std=c99
(base) user@ubuntu:~/Desktop/OS/NiuKe$ ./test 
pid : 3274
i am parent process, pid : 3273, ppid : 2077
i : 0 , pid : 3273
i am child process, pid : 3274, ppid : 3273
i : 0 , pid : 3274
i : 1 , pid : 3273
i : 1 , pid : 3274
i : 2 , pid : 3273
i : 2 , pid : 3274
```

这个程序比较简单，唯一值得注意的是输出结果的这个语句`i : 0 , pid : 3273`。

原因是fork之后父进程先执行还是子进程先执行不确定，取决于内核所使用的调度算法。


`program`

```c
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
    int n=2;
    for(int i=0;i<n;i++)
    {
        fork();
        printf("A\n");
    }
    return 0;
}
```
输出结果

```bash
(base) user@ubuntu:~/Desktop/OS/NiuKe$ gcc -o fork fork.c  -std=c99
(base) user@ubuntu:~/Desktop/OS/NiuKe$ ./fork 
A
A
A
A
A
A
```
代码共打印6个A，创建3个子进程。
![在这里插入图片描述](https://img-blog.csdnimg.cn/faee392784e04e49b7113371808050b9.png)
【注意】：'\n':会自动刷新缓冲区。即**遇到\n会自动刷新缓冲区**。


`program`

我们把上面程序的\n删除掉在试一下。

```c
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
    int n=2;
    for(int i=0;i<n;i++)
    {
        fork();
        printf("A");
    }
    return 0;
}
```
输出结果
```bash
(base) user@ubuntu:~/Desktop/OS/NiuKe$ gcc -o fork fork.c  -std=c99
(base) user@ubuntu:~/Desktop/OS/NiuKe$ ./fork 
AAAA(base) user@ubuntu:~/Desktop/OS/NiuKe$ AAAA
```

代码共打印8个A。

![在这里插入图片描述](https://img-blog.csdnimg.cn/b35d550c30dd4e238fc598ea4576aadb.png)


# 工作原理
- Linux 的 fork() 使用是通过**写时拷贝 (copy- on-write) 实现**。写时拷贝是一种可以推迟甚至避免拷贝数据的技术。
- 内核此时并不复制整个进程的地址空间，而是让父子进程共享同一个地址空间，只有在需要写入的时候才会复制地址空间，从而使各个进程拥有各自的地址空间。即**资源的复制是在需要写入的时候才会进行，在此之前，只有以只读方式共享**。
- fork之后父子进程共享文件。fork产生的子进程与父进程有相同的文件描述符，指向相同的文件表，引用计数增加，共享文件偏移指针。

# GDB 多进程调试
- 使用 GDB 调试的时候，GDB 默认只能跟踪一个进程，可以在 fork 函数调用之前，通过指令设置 GDB 调试工具跟踪父进程或者是跟踪子进程，**默认跟踪父进程**。

- 设置调试父进程或者子进程：`set follow-fork-mode [parent（默认）| child]`。


- 设置调试模式：`set detach-on-fork [on | off]`
默认为 `on`，表示调试当前进程的时候，其它的进程继续运行，如果为 `off`，调试当前进程的时候，其它进程被 `GDB` 挂起。

- 查看调试的进程：`info inferiors`。

- 切换当前调试的进程：`inferior Num`。

- 使进程脱离 GDB 调试：`detach inferiors Num`。


参考资料：[89-多进程开发：GDB多进程调试](https://blog.csdn.net/Edward_LF/article/details/124444315?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522167385630016800188550081%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=167385630016800188550081&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduend~default-1-124444315-null-null.142^v71^insert_chatgpt,201^v4^add_ask&utm_term=GDB%E8%B0%83%E8%AF%95%E5%A4%9A%E8%BF%9B%E7%A8%8B&spm=1018.2226.3001.4187)