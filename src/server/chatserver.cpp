#include "chatserver.hpp"
#include "chatservice.hpp"
#include <functional>
#include "json.hpp"
#include <string>
#include <muduo/base/Logging.h>
using namespace std;
using namespace placeholders;
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop *loop, const InetAddress &listenAddr, const string &nameArg)
    : _server(loop, listenAddr, nameArg), _loop(loop)
{
    // 注册链接事件回调
    _server.setConnectionCallback(bind(&ChatServer::onConnection, this, _1));
    // 注册信息事件的回调 注意 &ChatServer::onMessage 是为了将函数转化为函数指针
    _server.setMessageCallback(bind(&ChatServer::onMessage, this, _1, _2, _3));
    // 一个IO线程，三个工作线程
    _server.setThreadNum(4);
}
// 启动服务
void ChatServer::start()
{
    _server.start();
}
// 连接相关事件
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    LOG_INFO << "do onConnection service!!!";
    if (!conn->connected())
    {
        // 客户端异常退出
        ChatService::getInstance()->clientCloseException(conn);
        conn->shutdown();
    }
}
// 处理用户读写事件
void ChatServer::onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time)
{
    LOG_INFO<< "do onMessage service!!!";
    // if (!conn->connected())
    // {
    //     conn->shutdown();
    // }
    string buf = buffer->retrieveAllAsString();
    json js = json::parse(buf);
    // 通过js["mesid"]获取=》业务handler
    auto handler = ChatService::getInstance()->getHandler(js["msgid"].get<int>());

    handler(conn, js, time);
}
