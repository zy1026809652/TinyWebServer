#ifndef EPOLLER_H
#define EPOLLER_H

#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <vector>

class Epoller {
public:
    explicit Epoller(int maxEvent = 1024);
    ~Epoller();
    bool AddFd(int fd, uint32_t events);
    bool ModFd(int fd, uint32_t events);
    bool DelFd(int fd);
    int Wait(int timeoutMs = -1);
    int GetEventFd(size_t i) const; //size_t保证不同平台一样的长度
    uint32_t GetEvents(size_t i) const;

private:
    int epollFd_;  // epoll_create()创建一个epoll对象，返回值为epollFd
    std::vector<struct epoll_event> events_;  // 检测到的事件的集合
};

#endif  // EPOLLER_H