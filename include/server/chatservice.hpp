#ifndef CHAT_SERVICE_H
#define CHAT_SERVICE_H
#include <unordered_map>
#include <functional>
#include <muduo/net/TcpConnection.h>
#include <mutex>
#include "json.hpp"
#include "usermodel.hpp"
#include "friendmodel.hpp"
#include "offllinemessagemodel.hpp"
#include "groupmodel.hpp"
using namespace std;
using namespace muduo;
using namespace muduo::net;
using json = nlohmann::json;
// 处理消息事件回调方法类型
using MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp time)>;
// 单例模式 懒汉式 静态单例模式
// 服务层代码
class ChatService
{
public:
    // 处理登录业务 ORM Object-relational mapping
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 处理注册业务
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 一对一聊天
    void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 获取消息对应的处理器
    MsgHandler getHandler(int msgId);
    // 获取单例对象的函数
    static ChatService * getInstance();
    // 客户端异常退出
    void clientCloseException(const TcpConnectionPtr &conn);
    // 服务器异常，业务重置方法
    void reset();
    // 添加好友业务
    void addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 创建群组业务
    void creatGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 加入群组
    void addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 群聊天业务
    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
private:
    ChatService();
    // ~ChatService();
    // 禁止外部拷贝构造
    ChatService(const ChatService &other)=delete;
    // 禁止外部赋值构造
    ChatService& operator=(const ChatService &other)=delete;
    // 储存消息id和其对于的业务处理方法
    unordered_map<int, MsgHandler>_msgHandlerMap;
    // 存储在线用户的通信连接
    unordered_map<int, TcpConnectionPtr> _userConnMap;
    // 调用user数据库
    UserModel _usermodel;
    // 调用offlinemessage数据库
    OfflineMsg _offlinemsg;
    // friend好友guanxi
    FriendModel _friendmode;
    // 定义互斥锁 保证_userConnMap安全
    mutex _connMtx;
    // groupmode
    GroupModel _groupmodel;
    
    
};

#endif