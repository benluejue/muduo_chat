#include "chatservice.hpp"
#include "public.hpp"
#include "user.hpp"
#include "group.hpp"


#include <muduo/base/Logging.h>
#include <string>
#include <vector>
using namespace muduo;
using namespace std;
ChatService *ChatService::getInstance()
{
    /**
     * 局部静态变量实现单例模式
     * 局部变量只在当前函数内有效，其他函数无法访问
     * 在第一次调用时候初始化，存储在静态存储区，生命周期从第一次被初始调用 一直到程序结束
     * 这种可能存在线程安全问题，可能会很多程序都调用这个函数，解决方法为在函数开始就调用这个
     */
    static ChatService service;
    return &service;
}

ChatService::ChatService()
{
    // 个人消息
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, _1,_2,_3)});

    // 群组消息
    _msgHandlerMap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({CREATE_GROUP_MSG, std::bind(&ChatService::creatGroup, this, _1, _2,_3)});

    // 连接redis服务器
    if( _redis.connect() )
    {
        // 设置上报消息的回调
        _redis.init_notify_handler(std::bind(&ChatService::handleRedisSubscribeMessage, this, _1, _2));
    }
    
}

// 获取消息对应的处理器
MsgHandler ChatService::getHandler(int msgId)
{
    // 错误日志， msgID没有对应的事件回调处理
    auto it = _msgHandlerMap.find(msgId);
    if (it == _msgHandlerMap.end())
    {
        // muduo库的错误日志打印
        // LOG_ERROR << "msgid:"<<msgId<<"can not be find handler!";
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp time)
        {
            LOG_ERROR << "msgid: " << msgId << " can not be find handler!";
        };
    }
    else
    {
        return _msgHandlerMap[msgId];
    }
}

// 处理登录业务
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    LOG_INFO << "do login service!!!";
    // 从js获得用户id和密码
    int id = js["id"].get<int>();
    string pwd = js["password"];
    // 然后查询用户
    User user = _usermodel.query(id);
    // 如果密码正确
    if (user.getId() == id && user.getPwd() == pwd)
    {
        if (user.getState() == "online")
        {
            // 不许重新登录
            json response;
            response[MSGID] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "the user has logined,please enter other id";
            response["id"] = user.getId();
            conn->send(response.dump());
        }
        else
        {
            // 登录成功 记录 用户连接信息 如果单独写这个，可能会在多个线程下同时运行 要考虑线程安全问题
            {
                // lock_guard在作用域结束时候，自动解锁
                // 我们希望在 这个作用域中结束时候，其他线程就可以调用这个_connMtx，而不是等到运行完所有代码才解锁
                lock_guard<mutex> lock(_connMtx);
                _userConnMap.insert({id, conn});
            }
            // 登录成功 向redis添加channel
            _redis.subscribe(id);
            // 登录成功，更新状态信息
            user.setState("online");
            _usermodel.updateState(user);
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();
            response["state"] = user.getState();
            // 查询该用户是否有离线信息
            vector<string> vec = _offlinemsg.query(id);
            if( !vec.empty() )
            {
                response["offlinemessage"] = vec;
                // 获取离线信息之后 删除
                _offlinemsg.remove(id);  
            }
            //  登录后 展示好友信息
            vector<User> userVec =  _friendmode.query(id);
            if( !userVec.empty() )
            {
                vector<string>vec2;
                for(User& user : userVec)
                {
                    json js;
                    js["id"]=user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    vec2.push_back(js.dump());
                }
                response["friend"]=vec2;
            }
            // 查询用户群组信息
            vector<Group> groupVec = _groupmodel.queryGroups(id);
            if( !groupVec.empty() )
            {
                vector<string>groupV;
                for(Group& group : groupVec)
                {
                    json js;
                    js["id"] = group.getId();
                    js["groupname"] = group.getGroupName();
                    js["groupdesc"] = group.getGroupDesc();
                    vector<string> userV;
                    for(GroupUser &user : group.getUsers())
                    {
                        json userjs;
                        userjs["id"] = user.getId();
                        userjs["name"] = user.getName();
                        userjs["state"] = user.getState();
                        userjs["role"] = user.getRole();
                        userV.push_back(userjs.dump());
                    }
                    js["users"] = userV;
                    groupV.push_back(js.dump());
                }
                response["group"] = groupV;
            }
            conn->send(response.dump());
        }
    }
    else
    {
        // 用户不存在或密码错误   者登录失败
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "the user is invalid!";
        conn->send(response.dump());
    }
}
// 处理注册业务
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    LOG_INFO << "do reg server!!!";
    // 从js提取名字，密码
    string name = js["name"];
    string pwd = js["password"];
    // 声明User，设置名字，密码 获取状态
    User user;
    user.setName(name);
    user.setPwd(pwd);
    bool state = _usermodel.insert(user);
    // 如果成功 返回response消息
    if (state)
    {
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump());
    }
    else
    {
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}
// 客户端异常退出
void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> lock(_connMtx);
        // 当这个线程退出时候，删除对应的连接_userConnMap
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); it++)
        {
            if (it->second == conn)
            {
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }
    // 用户注销 取消订阅
    _redis.unsubscribe(user.getId());
    if (user.getId() != -1)
    {
        user.setState("offline");
        _usermodel.updateState(user);
    }
}
// 一对一聊天
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    LOG_INFO << "oneChat service!!!";
    int toid = js["toid"].get<int>();
    // 锁的力度要小，lock_guard出了作用域就释放了锁
    {
        lock_guard<mutex>lock(_connMtx);
        // 查找toid是否在线
        auto it = _userConnMap.find(toid);
        // 在线
        if( it != _userConnMap.end() ) 
        {
            // 服务器主动推送消息给toid用户
            it->second->send(js.dump());
            return;
        } 
    }
    // 如果不在线，可能是不在一个服务器上 直接使用redis转发
    User user = _usermodel.query(toid);
    if(user.getState() == "online")
    {
        _redis.publish(toid, js.dump());
        return ;
    }
    // 插入离线信息
    _offlinemsg.insert(toid, js.dump());
}
// 处理服务器的ctrl+c的 异常退出
void ChatService::reset()
{
    // 将所有的用户状态设置为offline
    _usermodel.restState();
}
// 添加友好 联合主键
void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();
    // 储存好友信息
    _friendmode.insert(userid, friendid);
}

 // 创建群组业务
void ChatService::creatGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];
    // 存储创建新的群组消息
    // 不填id，会自动生成，因为他是主键id
    Group group(-1, name, desc);
    if(_groupmodel.creatGroup(group))
    {
        // 存储 群组 创建人 信息
        _groupmodel.addGroup(userid, group.getId(), "creator");
    }
    
}
// 加入群组
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    //  _groupmodel.addGroup(userid, group.getId(), "creator");
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    _groupmodel.addGroup(userid, groupid, "normal");
}
// 群组聊天业务
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    // 查询除了 自己 以外的其他群友id
    // 自己的id
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    string msg = js["msg"];
    vector<int>userids = _groupmodel.queryGroupUser(userid, groupid);

    lock_guard<mutex>lock(_connMtx);
    for(auto user:userids)
    {
        int toid = user;
        // 查找toid是否在线
        auto it = _userConnMap.find(toid);
        // 在线
        if( it != _userConnMap.end() ) 
        {
            // 服务器主动推送消息给toid用户
            it->second->send(js.dump());
        }else
        {
            User auser = _usermodel.query(user);
            if(auser.getState() == "online")
            {
                _redis.publish(toid, js.dump());
            }else
            {
                // 插入离线信息
                _offlinemsg.insert(toid, js.dump());
            }
            
        }
    }
}
// 主动退出
void ChatService::loginout(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    {
        lock_guard<mutex> lock(_connMtx);
        auto it = _userConnMap.find(userid);
        if( it != _userConnMap.end() )
        {
            _userConnMap.erase(it);
        }
    }
    // 用户注销，就相当于下线 在redis中取消定于通道
    _redis.unsubscribe(userid);
    // 更新用户信息
    User user(userid, "", "", "offline");
    _usermodel.updateState(user);
}
// 给redis的预制回调函数
// 相当于redis消息发过来了，在这里转发一下
void ChatService::handleRedisSubscribeMessage(int userid, string msg)
{
    json js = json::parse(msg.c_str());
    lock_guard<mutex> lock(_connMtx);
    auto it = _userConnMap.find(userid);
    if( it != _userConnMap.end() )
    {
        it->second->send(js.dump());
        return;
    }
    _offlinemsg.insert(userid, msg);
}


