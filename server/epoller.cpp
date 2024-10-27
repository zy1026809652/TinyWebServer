#include "epoller.h"  // 包含自定义的头文件

/*epoll_event 结构体定义
typedef union epoll_data {
    void        *ptr;
    int          fd;
    uint32_t     u32;
    uint64_t     u64;
} epoll_data_t;

struct epoll_event {
    uint32_t     events;    // Epoll事件
    epoll_data_t data;      // 用户数据
};*/

// 创建Epoller类的对象时，初始化epoll实例
// 参数：maxEvents - epoll_wait可以返回的最大事件数
Epoller::Epoller(int maxEvents) : epollFd_(epoll_create(512)), events_(maxEvents) {
    // 确保epollFd_创建成功，并且events_向量大小大于0
    assert(epollFd_ >= 0 && events_.size() > 0);
}

// 析构函数，释放资源
Epoller::~Epoller() {
    close(epollFd_);  // 关闭epoll文件描述符
}

// 添加文件描述符到epoll实例中
// 参数：fd - 文件描述符
//       events - 要监视的事件（如EPOLLIN, EPOLLOUT等）
// 返回值：成功返回true，失败返回false
bool Epoller::AddFd(int fd, uint32_t events) {
    if (fd < 0) return false;  // 如果文件描述符无效，直接返回false

    // 初始化一个epoll_event结构体
    epoll_event ev = {0};
    ev.data.fd = fd;     // 设置关联的文件描述符
    ev.events = events;  // 设置要监视的事件

    // 使用epoll_ctl系统调用，操作类型为EPOLL_CTL_ADD
    // 如果成功，返回0；否则返回-1
    return 0 == epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &ev);
}

// 修改文件描述符在epoll实例中的事件
// 参数：fd - 文件描述符
//       events - 新的事件
// 返回值：成功返回true，失败返回false
bool Epoller::ModFd(int fd, uint32_t events) {
    if (fd < 0) return false;  // 如果文件描述符无效，直接返回false

    // 初始化一个epoll_event结构体
    epoll_event ev = {0};
    ev.data.fd = fd;     // 设置关联的文件描述符
    ev.events = events;  // 设置新的事件

    // 使用epoll_ctl系统调用，操作类型为EPOLL_CTL_MOD
    // 如果成功，返回0；否则返回-1
    return 0 == epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &ev);
}

// 从epoll实例中删除文件描述符
// 参数：fd - 文件描述符
// 返回值：成功返回true，失败返回false
bool Epoller::DelFd(int fd) {
    if (fd < 0) return false;  // 如果文件描述符无效，直接返回false

    // 初始化一个epoll_event结构体
    epoll_event ev = {0};

    // 使用epoll_ctl系统调用，操作类型为EPOLL_CTL_DEL
    // 如果成功，返回0；否则返回-1
    return 0 == epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &ev);
}

// 等待并获取就绪的事件
// 参数：timeoutMs - 等待超时时间（毫秒），-1表示无限等待
// 返回值：返回就绪的事件数量
int Epoller::Wait(int timeoutMs) {
    // 使用epoll_wait系统调用，等待时间由timeoutMs指定
    // &events_[0] 是events_向量的首地址
    // static_cast<int>(events_.size()) 是events_向量的大小
    return epoll_wait(epollFd_, &events_[0], static_cast<int>(events_.size()), timeoutMs);
}

// 获取第i个就绪事件的文件描述符
// 参数：i - 事件索引
// 返回值：文件描述符
int Epoller::GetEventFd(size_t i) const {
    // 确保索引有效
    assert(i < events_.size() && i >= 0);

    // 返回events_向量中第i个事件的文件描述符
    return events_[i].data.fd;
}

// 获取第i个就绪事件的事件类型
// 参数：i - 事件索引
// 返回值：事件类型
uint32_t Epoller::GetEvents(size_t i) const {
    // 确保索引有效
    assert(i < events_.size() && i >= 0);

    // 返回events_向量中第i个事件的事件类型
    return events_[i].events;
}