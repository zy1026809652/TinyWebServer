#include "webserver.h"  // 包含自定义的头文件

// WebServer类的构造函数
// 参数：
//   port - 监听的端口号
//   timeoutMs - epoll_wait的超时时间（毫秒）
WebServer::WebServer(int port, int timeoutMs) : port_(port), timeoutMs_(timeoutMs), 
                    epoller_(new Epoller()) {
    // 初始化套接字
    InitSocket_();
}

// 初始化套接字
bool WebServer::InitSocket_() {
    // 创建一个IPv4的TCP套接字
    listenFd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd_ == -1) {
        perror("socket");  // 打印错误信息
        return false;  // 返回false表示初始化失败
    }

    // 设置套接字选项，允许地址重用
    int optval = 1;
    setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // 设置服务器地址结构
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;  // 地址族为IPv4
    saddr.sin_addr.s_addr = INADDR_ANY;  // 绑定到所有可用的网络接口
    saddr.sin_port = htons(port_);  // 将端口号转换为网络字节序

    // 绑定套接字到指定的地址和端口
    int ret = bind(listenFd_, (struct sockaddr*)&saddr, sizeof(saddr));
    if (ret == -1) {
        perror("bind");  // 打印错误信息
        return false;  // 返回false表示绑定失败
    }

    // 开始监听连接请求，最大连接数为128
    ret = listen(listenFd_, 128);
    if (ret == -1) {
        perror("listen");  // 打印错误信息
        return false;  // 返回false表示监听失败
    }

    // 将监听套接字添加到epoll实例中，监视可读事件
    if (!epoller_->AddFd(listenFd_, EPOLLIN)) {
        perror("epoll_ctl add");
        return false;  // 返回false表示添加失败
    }

    // 主循环
    while (1) {
        // 等待并获取就绪的事件
        int num = epoller_->Wait(timeoutMs_);
        if (num == -1) {
            perror("epoll_wait");  // 打印错误信息
            exit(-1);  // 如果epoll_wait失败，退出程序
        }
        printf("num = %d\n", num);  // 打印就绪事件的数量

        // 遍历所有就绪的事件
        for (int i = 0; i < num; ++i) {
            int curfd = epoller_->GetEventFd(i);  // 获取当前事件的文件描述符

            // 如果是监听套接字有新的连接请求
            if (curfd == listenFd_) {
                // 接受新的连接
                struct sockaddr_in cliaddr;
                socklen_t len = sizeof(cliaddr);
                int cfd = accept(listenFd_, (struct sockaddr*)&cliaddr, &len);

                // 获取客户端的IP地址
                char cliIp[16];
                inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, cliIp, sizeof(cliIp));
                unsigned short cliPort = ntohs(cliaddr.sin_port);  // 获取客户端的端口号

                // 输出客户端的信息
                printf("client's ip is %s, and port is %d\n", cliIp, cliPort);

                // 设置新连接的文件描述符为非阻塞
                int flag = fcntl(cfd, F_GETFL);
                flag |= O_NONBLOCK;  // 添加O_NONBLOCK标志
                fcntl(cfd, F_SETFL, flag);

                // 将新连接的文件描述符添加到epoll实例中，监视可读事件，并设置边缘触发模式
                if (!epoller_->AddFd(cfd, EPOLLIN | EPOLLET)) { 
                    //-> 运算符使得通过指针访问类成员更加简洁和直观。直接使用 epoller_->AddFd 比 (*epoller_).AddFd 更易读。
                    perror("epoll_ctl add client fd");
                    close(cfd);  // 如果添加失败，关闭文件描述符
                    continue;  // 跳过本次循环
                }
            } else {  // 如果是已连接的客户端发送数据
                // 检查事件类型
                if (epoller_->GetEvents(i) & EPOLLOUT) {
                    // 如果是写事件，这里暂时不处理，继续下一个事件
                    continue;
                }

                // 读取客户端发送的数据
                char buf[5];
                int len = 0;
                while ((len = read(curfd, buf, sizeof(buf))) > 0) {
                    // 将读取的数据输出到标准输出
                    write(STDOUT_FILENO, buf, len);
                    // 将读取的数据回传给客户端
                    write(curfd, buf, len);
                }

                // 如果读取长度为0，表示客户端关闭了连接
                if (len == 0) {
                    printf("client close...\n");
                    // 从epoll实例中删除该文件描述符
                    epoller_->DelFd(curfd);
                    // 关闭文件描述符
                    close(curfd);
                } else if (len == -1) {  // 读取失败
                    if (errno == EAGAIN) {
                        // 如果是EAGAIN错误，表示没有更多数据可读
                        printf("read over ...\n");
                    } else {
                        // 其他错误，打印错误信息并退出
                        perror("read\n");
                        exit(-1);
                    }
                }
            }
        }
    }

    // 关闭监听套接字
    close(listenFd_);
    return true;  // 返回true表示初始化成功
}