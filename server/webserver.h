#ifndef WEBSERVER_H
#define WEBSERVER_H

// 预处理器指令：防止头文件被多次包含
// 如果WEBSERVER_H没有被定义，则定义它并继续处理下面的内容

#ifndef WEBSERVER_H
#define WEBSERVER_H

// 包含标准输入输出库，提供了printf和perror等函数，用于打印信息和错误信息
#include <stdio.h>
// 包含字符处理函数头文件，用于isprint等函数
#include <ctype.h>
// 包含网络编程相关的函数和数据结构，例如inet_ntop和sockaddr_in，用于处理IP地址和端口
#include <arpa/inet.h>
// 包含Unix标准库函数，如close, read, write等，用于文件描述符操作
#include <unistd.h>
// 包含内存管理函数，如malloc, free等
#include <stdlib.h>
// 包含字符串处理函数，如strlen, strcpy等，用于处理字符串
#include <string.h>
// 包含C++标准库中的智能指针，例如std::unique_ptr，用于自动管理资源，防止内存泄漏
#include <memory>
// 包含自定义的Epoller类，用于管理多个文件描述符上的I/O事件
#include "epoller.h"

// 定义WebServer类
class WebServer {
public:
    // 构造函数
    // 参数：
    //   port - 服务器监听的端口号
    //   timeoutMs - epoll_wait的超时时间（毫秒）
    // 功能：初始化WebServer对象，并设置端口号和超时时间
    WebServer(int port, int timeoutMs);

    // 析构函数
    // 功能：释放资源
    // 当前实现为空，但可以在将来添加清理代码
    ~WebServer() {}

    // 启动服务器的方法
    // 当前实现为空，但可以在将来添加启动逻辑
    // void Start();

private:
    // 初始化套接字的方法
    // 功能：创建监听套接字，绑定到指定端口，并开始监听连接请求
    // 返回值：成功返回true，失败返回false
    bool InitSocket_();

    // 成员变量
    int timeoutMs_;  // epoll_wait的超时时间（毫秒）
    int port_;       // 服务器监听的端口号
    int listenFd_;   // 监听套接字的文件描述符
    std::unique_ptr<Epoller> epoller_;  // 智能指针，管理Epoller对象的生命周期
};

// 结束预处理器条件编译块
#endif  // WEBSERVER_H