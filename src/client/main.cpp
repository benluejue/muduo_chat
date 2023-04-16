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
#include "group.hpp"
#include "user.hpp"
#include "public.hpp"
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

// 记录当前系统登录的用户信息
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
                cin>>id;
                cin.get();
                char pwd[50] = {0};
                cin.getline(pwd, 50);
                
                js["id"]=id;
                js["msgid"] = LOGIN_MSG;
                js["password"] = pwd;
                string request = js.dump();
                if( send(clientfd, request.c_str(), strlen( request.c_str() ), 0) == -1)
                    cerr<<"send fail!!! what you send is "<<request<<endl;
                
            }
            case 3:
            {
                break;
            }
        }
    }

    return 0;
}
