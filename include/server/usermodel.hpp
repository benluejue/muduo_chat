#ifndef USERMODEL_H
#define USERMODEL_H
#include "user.hpp"
// user表的操作类
class UserModel{
public:
    // 注册时候插入信息
    bool insert(User& user);
    // 根据用户主键ID查询信息
    User query(int id);
    // 更新用户信息
    bool updateState(User user);
    // 因服务器退出，将所有账号状态设为离线
    bool restState();
};
#endif

