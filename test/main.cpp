#include "../server/webserver.h"

int main() {
    WebServer webserver(1316, -1);  // 设置端口, 超时时间
    // webserver.Start();
    return 0;
}