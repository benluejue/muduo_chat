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
int main(int argc, char **argv)
{
    if( argc < 3)
    {
        cerr<<"Usage ./ChatServer 127.0.0.1 6000 or 6002"<<endl;
        exit(-1);
    }
    // 中断信号触发
    signal(SIGINT, resetHandler);
    EventLoop loop; // 相当于创建一个epoll
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);
    InetAddress addr(ip, port);
    ChatServer server(&loop, addr, "chat server");

    server.start(); // 将listenfd添加到 epoll_ctl
    loop.loop();    // epoll_wait以阻塞方式等待用户连接
    return 0;
}