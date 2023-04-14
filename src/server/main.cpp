#include "chatserver.hpp"
#include "chatservice.hpp"
#include <iostream>
#include <signal.h>
using namespace std;
// 处理服务器ctrl+c结束后，处理user的状态信息，使其全部离线
void resetHandler(int)
{
    ChatService::getInstance()->reset();
    exit(0);
}
int main()
{
    // 中断信号触发
    signal(SIGINT, resetHandler);
    EventLoop loop; // 相当于创建一个epoll
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "chat server");

    server.start(); // 将listenfd添加到 epoll_ctl
    loop.loop();    // epoll_wait以阻塞方式等待用户连接
    return 0;
}