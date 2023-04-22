#include <iostream>
#include <stdio.h>
#include <vector>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
// #include <string>和#include <string.h>的区别
// string是c++的头文件，
// 封装了string的用法， string.h是c文件头，封装了strcpy()等
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>
#include "group.hpp"
#include "user.hpp"
#include "public.hpp"
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

// 记录当前系统登录的用户信息 不能一直从服务器读，要求保存在本地
User g_currentUser;
// 记录当前用户的好友列表信息
vector<User> g_currentUserFriendList;
// 记录当前用户的群组信息列表
vector<Group> g_currentUserGroupList;
// 显示当前登录用户的基本信息
void showCurrentUserData();

// 接受线程
void readTaskHandler(int clientfd);
// 获取系统时间
string getCurrentTime();
// 聊天主页面程序
void mainMeau();

// 输入 ./Client 127.0.0.1 6000
// 聊天客户端实现， main线程 发送 线程 子线程为 接收 线程
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Usage ./Client <IP> <port>" << endl;
        exit(-1);
    }
    int clientfd;
    // PF_INET 地址簇 SOCK_STREAM套接字类型
    clientfd = socket(PF_INET, SOCK_STREAM, 0);
    if (clientfd == -1)
        cerr << "socket fail" << endl;
    struct sockaddr_in serv_adr;
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));
    if (connect(clientfd, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
    {
        cerr << "connect fail" << endl;
        close(clientfd);
        exit(-1);
    }
    else
    {
        cout << "connect...." << endl;
    }
    // main线程用于接受用户输入
    for (;;)
    {
        // 显示登录菜单
        cout << "================================" << endl;
        cout << "1. login" << endl;
        cout << "2. register" << endl;
        cout << "3. quit" << endl;
        cout << "================================" << endl;
        cout << "choice:" << endl;
        int choice = 0;
        cin>>choice;
        cin.get(); // 当打第一个数字之后，当要接受以后的数字，要清除之前的剩余的回车符

        switch (choice)
        {
            case 1: // 登录业务
            {
                // 输入id password
                json js;
                int id = 0;
                cout<<"input your id: ";
                cin>>id;
                cin.get();
                char pwd[50] = {0};
                cout<<"input your password: ";
                cin.getline(pwd, 50);
                
                js["id"]=id;
                js["msgid"] = LOGIN_MSG;
                js["password"] = pwd;
                string request = js.dump();
                if( send(clientfd, request.c_str(), strlen( request.c_str() )+1, 0) == -1)
                    cerr<<"send fail!!! what you send is "<<request<<endl;
                else
                {
                    char buffer[1024]={0};
                    if( recv(clientfd, buffer, 1024, 0) == -1)
                    {
                        cerr<<"recv fail! login fail!!"<<endl;
                    }
                    else
                    {
                        json request = json::parse(buffer);
                        int response_errn =  request["errno"].get<int>();
                        if( response_errn == 2)
                        {
                            // 不许重新登录
                            cerr<<request["errmsg"]<<endl;
                        }
                        else if( response_errn == 0)
                        {
                            cout<<"login sucess!!!!"<<endl;
                            g_currentUser.setId(request["id"].get<int>());
                            g_currentUser.setName( request["name"] );
                            g_currentUser.setState( request["state"]);
                            
                            if(request.contains("friend"))
                            {
                                cout<<"find friend a"<<endl;
                                vector<string> friends = request["friend"];
                                for(auto afriend:friends)
                                {
                                    User user;
                                    json js = json::parse(afriend);
                                    user.setId(js["id"].get<int>());
                                    user.setName(js["name"]);
                                    user.setState(js["state"]);
                                    g_currentUserFriendList.push_back(user);
                                }
                            }
                            if( request.contains("group") )
                            {
                                vector<string> groups = request["group"];
                                for(auto agroup:groups)
                                {
                                    Group group;
                                    json js = json::parse(agroup);
                                    group.setId(js["id"].get<int>());
                                    group.setGroupName(js["groupname"]);
                                    group.setGroupDesc(js["groupdesc"]);
                                    vector<string> groupusers = js["users"];
                                    for( auto agroupuser:groupusers)
                                    {
                                        GroupUser user;
                                        json js = json::parse(agroupuser);
                                        user.setId(js["id"].get<int>());
                                        user.setName(js["name"]);
                                        user.setRole(js["role"]);
                                        user.setState(js["state"]);
                                        // 先get再 push_back
                                        group.getUsers().push_back(user);
                                    }
                                    g_currentUserGroupList.push_back(group);
                                }
                            }
                            // 显示登录用户的基本信息
                            showCurrentUserData();
                            // 显示个人的离线信息
                            if( request.contains("offlinemessage") )
                            {
                                vector<string> offlinemsgs;
                                for(auto offlinemsg:offlinemsgs)
                                {
                                    json js = json::parse(offlinemsg);
                                    cout<<js["from"]<<"("<<js["id"]<<")"<<" said "<<js["msg"]<<endl;
                                }
                            }
                            // 登录成功之后，启动线程负责接受数据
                            std::thread t(readTaskHandler, clientfd);
                            t.detach();
                            // 返回主页面
                            mainMeau();
                        }
                        else if( response_errn == 1)
                        {
                            // 用户不存在或密码错误   者登录失败
                            cerr<<request["errmsg"]<<endl;
                        }
                    }
                }
                
            }
            break;
            case 2:
            {
                // 注册业务
                json js;
                char name[50] = {0};
                char pwd[50] = {0};
                cout<<"cin user name: ";
                // 好处遇到空格不会回车
                cin.getline(name, 50);
                cout<<"cin user password: ";
                cin.getline(pwd, 50);
                js["msgid"] = REG_MSG;
                js["name"] = name;
                js["password"] = pwd;
                string request = js.dump();
                if( send(clientfd, request.c_str(), strlen(request.c_str())+1, 0) == -1)
                {
                    cerr<<"login send fail!! "<<request<<endl;
                }
                else
                {
                    char buffer[1024]={0};
                    if( recv(clientfd, buffer, 1024, 0) == -1)
                    {
                        // 和cout的区别 cerr不会被缓冲，可以直接输出出来
                        cerr<<"recv fail!"<<endl;
                    }else
                    {
                        // string转js
                        json response = json::parse(buffer);
                        int response_errn = response["errno"].get<int>();
                        // 注册成功
                        if(response_errn == 0)
                        {
                            cout<<"your id is "<<response["id"]<<" dont forget it"<<endl;
                        }
                        else
                        {
                            cerr<<"reg fail"<<endl;
                        }
                    }
                }

            }
            break;
            case 3:
            {
                exit(0);
            }
            default:
                cerr<<"invalue input!"<<endl;
                break;
        }
    }

    return 0;
}

void showCurrentUserData()
{
    cout<<"============login user=============="<<endl;
    cout<<"current login user id: "<<g_currentUser.getId()<<" name: "<<
            g_currentUser.getName()<<endl;
    cout<<"--------------friend list------------"<<endl;
    // vector<User> g_currentUserFriendList;
    if(!g_currentUserFriendList.empty() )
    {
        cout<<"id"<<" name     state"<<endl;
        for(auto afriend:g_currentUserFriendList)
        {
            cout<<afriend.getId()<<" "<<afriend.getName()<<" "<<afriend.getState()<<endl;
        }
    }
    
    cout<<"-------------group list------------"<<endl;
     if (!g_currentUserGroupList.empty())
    {   
        cout<<"id"<<" name     state"<<endl;
        for (Group &group : g_currentUserGroupList)
        {
            cout << group.getId() << " " << group.getGroupName() << " " << group.getGroupDesc() << endl;
            for (GroupUser &user : group.getUsers())
            {
                cout << user.getId() << " " << user.getName() << " " << user.getState()
                     << " " << user.getRole() << endl;
            }
        }
    }
}

void mainMeau()
{
    cout<<"================mainMeau====================="<<endl;
}

void readTaskHandler(int clientfd)
{
    cout<<"================="<<endl;
}
