#include "usermodel.hpp"
#include "db.hpp"
#include <iostream>
using namespace std;
// user表的增加方法
bool UserModel::insert(User &user)
{
    // 1组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into user(name, password,state) values('%s', '%s', '%s')",
            user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            // 获取插入成功的主键
            user.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}
// 根据用户主键ID查询信息
User UserModel::query(int id)
{
    // 1组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select * from user where id = %d",
            id);
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            // row 获取一行 因为是主键 所以肯定只有一行
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPwd(row[2]);
                user.setState(row[3]);
                mysql_free_result(res);
                return user;
            }
        }
    }
    return User();
}

// 更新用户信息
bool UserModel::updateState(User user)
{
    // 1组装sql语句
    char sql[1024] = {0};
    // c_str()将string风格转化为c语言的char*风格
    sprintf(sql, "update user set state='%s' where id = %d",
            user.getState().c_str(), user.getId());
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            return true;
        }
    }
    return false;
}

// 因服务器退出，将所有账号状态设为离线
bool UserModel::restState()
{
    // 1组装sql语句
    char sql[1024] = {0};
    // c_str()将string风格转化为c语言的char*风格
    sprintf(sql, "update user set state='offline' where state = 'online'");
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            return true;
        }
    }
    return false;
}
