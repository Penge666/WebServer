# Linux高并发服务器开发项目

## 启动服务器

启动命令

```bash
(base) user@ubuntu:~/Desktop/OS/NiuKe$ g++ *.cpp -pthread
(base) user@ubuntu:~/Desktop/OS/NiuKe$ ./a.out 10002
create the 0th thread
create the 1th thread
create the 2th thread
create the 3th thread
create the 4th thread
create the 5th thread
create the 6th thread
create the 7th thread
got 1 http line: Host: 192.168.68.141:10009
got 1 http line: User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/109.0
oop! unknow header User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/109.0
got 1 http line: Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8
oop! unknow header Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8
got 1 http line: Accept-Language: en-US,en;q=0.5
oop! unknow header Accept-Language: en-US,en;q=0.5
...
```

在Firefox中输入IP，端口号及访问的文件路径

![image-20230201104433322](E:\Lab\cs144\Linux高性能服务器\Project\ReadMe.assets\image-20230201104433322.png)

[Bug]：You do not have permission to get file from this server. 

原因：权限不够            

增加文件权限：`chmod 777 file`

## 压力测试

项目使用Webbench进行压力测试。

 Webbench是 Linux 上一款知名的、优秀的 web 性能压力测试工具。它是由Lionbridge公司开发。

基本原理：Webbench 首先 fork 出多个子进程，每个子进程都循环做 web 访问测试。子进程把访问的结果通过pipe 告诉父进程，父进程做最终的统计结果。

```bash
(base) user@ubuntu:~/Desktop/OS/NiuKe/test_presure/webbench-1.5$ ./webbench -c 100 -t 5 http://192.168.68.141:10009/index.html
Webbench - Simple Web Benchmark 1.5
Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.

Benchmarking: GET http://192.168.68.141:10009/index.html
100 clients, running 5 sec.

Speed=533868 pages/min, 1414750 bytes/sec.
Requests: 44489 susceed, 0 failed.
```

- -c 表示客户端数

- -t 表示时间