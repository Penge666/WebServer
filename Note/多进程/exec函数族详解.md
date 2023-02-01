@[TOC]

# exec介绍
通过命令查看帮助：`man 3 exec`

![在这里插入图片描述](https://img-blog.csdnimg.cn/65f6f5909ef84dcea6dabcc55dd98e7c.png)

**exec 函数族**的作用是根据指定的文件名找到可执行文件，并用它来取代调用进程的内容，换句话说，**就是在调用进程内部执行一个可执行文件**。

exec 函数族的函数执行成功后不会返回，因为调用进程的实体，包括代码段，数据段和堆栈等都已经被新的内容取代，只留下进程 ID 等一些表面上的信息。只有调用失败了，它们才会返回 -1，从原程序的调用点接着往下执行。

![在这里插入图片描述](https://img-blog.csdnimg.cn/2c7b58d788fc41f099b3781e7048a242.png)
图片参考自：[【Linux】进程控制（exec函数族）的理解和使用](https://blog.csdn.net/m0_46606290/article/details/123618822?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522167383661416800188574733%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=167383661416800188574733&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~top_positive~default-1-123618822-null-null.142^v71^insert_chatgpt,201^v4^add_ask&utm_term=exec%E5%87%BD%E6%95%B0&spm=1018.2226.3001.4187)

**进程程序替换并不会创建新的进程，它只会加载程序的代码和数据，去替换原来的进程。**



# exec族

```cpp
int execl(const char *path, const char *arg, .../* (char *) NULL */);
int execlp(const char *file, const char *arg, ... /* (char *) NULL */);
int execle(const char *path, const char *arg, .../*, (char *) NULL, char * const envp[] */);
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execvpe(const char *file, char *const argv[], char *const envp[]);
int execve(const char *filename, char *const argv[], char *const envp[]);
```
返回值
- exec 函数族的函数执行成功后不会返回。
- 调用失败了，返回 -1。

后面跟不同参数，代表不同含义

- l(list) ：参数地址列表，以空指针结尾

- v(vector) ：存有各参数地址的指针数组的地址

- p(path) ：按 PATH 环境变量指定的目录搜索可执行文件，可用env查看现有的环境变量

- e (environment)：存有环境变量字符串地址的指针数组的地址

==前面2个用的比较多==

# execl函数

- int execl(const char *path, const char *arg, .../* (char *) NULL */);

  - path：需要指定的执行的文件的路径或者名称

  - arg：是执行可执行文件所需要的参数列表。第一个参数一般没有什么作用，为了方便，一般写的是执行的程序的名称，从第二个参数开始往后，就是程序执行所需要的的参数列表，参数最后需要以NULL结束（哨兵）


说明：初始文件夹中存在 `hello.c`,`hello`2个文件，内容是输出随便一个语句。

`program`
```c
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    // 创建一个子进程，在子进程中执行exec函数族中的函数
    pid_t pid = fork();
    if(pid > 0) {
        // 父进程
        printf("i am parent process, pid : %d\n",getpid());
        sleep(1);
    }else if(pid == 0) {
        execl("hello","hello",NULL);
        perror("execl");
        printf("i am child process, pid : %d\n", getpid());
    }
    for(int i = 0; i < 3; i++) {
        printf("i = %d, pid = %d\n", i, getpid());
    }
    return 0;
}
```

运行结果
```bash
(base) user@ubuntu:~/Desktop/OS/NiuKe$ gcc -o test test.c -std=c99
(base) user@ubuntu:~/Desktop/OS/NiuKe$ ./test 
i am parent process, pid : 4426
Hello world
i = 0, pid = 4426
i = 1, pid = 4426
i = 2, pid = 4426
```
可以看到，子进程的内容（用户区）被替换，打印的是hello可执行文件中的内容。

# execlp函数
- int execlp(const char *file, const char *arg, ... /* (char *) NULL */);

  - 会到环境变量中查找指定的可执行文件，如果找到了就执行，找不到就执行不成功

  - file：只需要提供名称（不需要提供路径）

`program`

```c
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
int main() {
    // 创建一个子进程，在子进程中执行exec函数族中的函数
    pid_t pid = fork();

    if(pid > 0) {
        // 父进程
        printf("i am parent process, pid : %d\n",getpid());
        sleep(1);
    }else if(pid == 0) {
        // 子进程
        execlp("ps", "ps", "aux", NULL);

        printf("i am child process, pid : %d\n", getpid());
    }
    for(int i = 0; i < 3; i++) {
        printf("i = %d, pid = %d\n", i, getpid());
    }
    return 0;
}
```
输出结果

```bash
(base) user@ubuntu:~/Desktop/OS/NiuKe$ gcc -o test test.c -std=c99
(base) user@ubuntu:~/Desktop/OS/NiuKe$ ./test 
i am parent process, pid : 4437
USER        PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
root          1  0.0  0.2 160048  9248 ?        Ss   Jan15   0:03 /sbin/init auto noprompt
root          2  0.0  0.0      0     0 ?        S    Jan15   0:00 [kthreadd]
root          3  0.0  0.0      0     0 ?        I<   Jan15   0:00 [rcu_gp]
root          4  0.0  0.0      0     0 ?        I<   Jan15   0:00 [rcu_par_gp]
root          6  0.0  0.0      0     0 ?        I<   Jan15   0:00 [kworker/0:0H-kb]
...
user       4438  0.0  0.0  41424  3568 pts/3    R+   00:51   0:00 ps aux
i = 0, pid = 4437
i = 1, pid = 4437
i = 2, pid = 4437
```
# execv函数
- int execv(const char *path, char *const argv[]);

  - argv：将运行参数都写在数组中

`program`
```c
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

int main() {
    // 创建一个子进程，在子进程中执行exec函数族中的函数
    pid_t pid = fork();

    if(pid > 0) {
        // 父进程
        printf("i am parent process, pid : %d\n",getpid());
        sleep(1);
    }else if(pid == 0) {
        // 子进程
        char* argv[] = {"hello", NULL};
        execv("hello", argv);
        perror("execv");
        printf("i am child process, pid : %d\n", getpid());

    }

    for(int i = 0; i < 3; i++) {
        printf("i = %d, pid = %d\n", i, getpid());
    }
    return 0;
}
```

输出结果


```bash
(base) user@ubuntu:~/Desktop/OS/NiuKe$ gcc -o test test.c -std=c99
(base) user@ubuntu:~/Desktop/OS/NiuKe$ ./test 
i am parent process, pid : 4453
Hello world
i = 0, pid = 4453
i = 1, pid = 4453
i = 2, pid = 4453
```


剩下的几个函数用法类似，就不11展示啦 ~