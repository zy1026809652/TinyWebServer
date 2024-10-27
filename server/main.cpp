#include "../server/webserver.h"

int main() {
    WebServer webserver(1316, -1);  // 设置端口, 超时时间
    // webserver.Start();
    return 0;
}
/*
    创建 epoll 实例：使用 epoll_create1(0) 创建一个 epoll 文件描述符。
打开文件：使用 open 打开一个文件，并获取文件描述符。
设置事件：创建一个 epoll_event 结构体 ev，并设置 events 字段为 EPOLLIN | EPOLLPRI，表示监视可读事件和优先级带外数据。将文件描述符存储在 data 字段中。
添加到 epoll：使用 epoll_ctl 将文件描述符添加到 epoll 实例中。
等待事件：使用 epoll_wait 等待事件发生，并处理就绪的事件。
处理事件：遍历 epoll_wait 返回的事件数组，检查每个事件的 events 字段，并根据不同的事件类型进行处理
*/