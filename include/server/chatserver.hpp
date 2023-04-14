#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <functional>
using namespace muduo;
using namespace muduo::net;
class ChatServer
{
public:
    // loop时间循环，listenAddr IP+addr nameArg服务器名字
    ChatServer(EventLoop *loop,
               const InetAddress &listenAddr,
               const string &nameArg);
    // 开启服务
    void start();

private:
    // 处理连接事件
    void onConnection(const TcpConnectionPtr &conn);
    // 处理用户读写事件
    void onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time);
    EventLoop *_loop;  // 指向事件循环
    TcpServer _server; // 实现服务器的类对象
};

#endif
