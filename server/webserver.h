#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <stdio.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <memory>
#include "epoller.h"

class WebServer {
public:
    WebServer(int port, int timeoutMs);
    ~WebServer() {}
    // void Start();
private:
    bool InitSocket_();
    int timeoutMs_;
    int port_;
    int listenFd_;
    std::unique_ptr<Epoller> epoller_;
};

#endif  // WEBSERVER_H