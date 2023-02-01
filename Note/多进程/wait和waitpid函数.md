@[TOC]
**进程回收**
- 在每个进程退出的时候，内核释放该进程所有的资源、包括打开的文件、占用的内存等。但是仍然为其保留一定的信息，这些信息主要主要指进程控制块`PCB`的信息（包括进程号、退出状态、运行时间等）
- 父进程可以通过调用wait或waitpid得到它的退出状态同时彻底清除掉这个进程。
- `wait()` 和 `waitpid()` 函数的功能一样，区别在于
  - `wait()` 函数会阻塞
  - `waitpid()` 可以设置是否阻塞，`waitpid()` 还可以指定等待哪个子进程结束

**注意**：一次`wait`或`waitpid`调用只能清理一个子进程，清理多个子进程应使用循环

# wait()函数
`man 2 wait`

![在这里插入图片描述](https://img-blog.csdnimg.cn/e7f7a89c91564f65aadbdc68f6adacf1.png)
- `pid_t wait(int *wstatus);`

  - 功能：等待任意一个子进程结束，如果任意一个子进程结束了，此函数会回收子进程的资源

  - 参数
`int *wstatus`：进程退出时的状态信息，传入的是一个`int`类型的地址，传出参数。
  - 返回值
成功：返回被回收的子进程的id
失败：`-1` (所有的子进程都结束，调用函数失败)


```c
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() 
{
    // 有一个父进程，创建5个子进程（兄弟）
    pid_t pid;

    // 创建5个子进程
    for(int i = 0; i < 5; i++) {
        pid = fork();
        // 避免嵌套重复生成子进程
        if(pid == 0) {
            break;
        }
    }
    if(pid > 0) {
        // 父进程
        while(1) {
            printf("parent, pid = %d\n", getpid());
            int ret = wait(NULL);
            if(ret == -1) {
                break;
            }
            printf("child die, pid = %d\n", ret);
            sleep(1);
        }

    } else if (pid == 0){
        // 子进程
         while(1) {
            printf("child, pid = %d\n",getpid());    
            sleep(1);       
         }
    }
    return 0; // exit(0)
}
```
`shell1`
```bash
(base) user@ubuntu:~/Desktop/OS/NiuKe$ gcc -o test test.c -std=c99
(base) user@ubuntu:~/Desktop/OS/NiuKe$ ./test 
child, pid = 5614
child, pid = 5615
child, pid = 5611
child, pid = 5612
child die, pid = 5611
child, pid = 5613
child, pid = 5614
child, pid = 5612
child, pid = 5615
parent, pid = 5610
child, pid = 5612
child, pid = 5614
child, pid = 5615
child, pid = 5613
child, pid = 5615
child, pid = 5612
child, pid = 5614
child, pid = 5613
child, pid = 5615
child, pid = 5613
child, pid = 5614
child, pid = 5612
child die, pid = 5612
...
```
`shell1`

```bash
(base) user@ubuntu:~$ kill -9 5611
(base) user@ubuntu:~$ kill -9 5612
(base) user@ubuntu:~$ kill -9 5613
(base) user@ubuntu:~$ kill -9 5614
(base) user@ubuntu:~$ kill -9 5615
```

# waitpid()函数