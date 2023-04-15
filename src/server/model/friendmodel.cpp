#include "friendmodel.hpp"
#include "db.hpp"

// 添加好友
void FriendModel::insert(int userid, int friendid)
{
    char sql[1024] = {0};
    // c_str()将string风格转化为c语言的char*风格
    sprintf(sql, "insert into friend values(%d, %d)",userid, friendid);
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

// 返回用户好友列表
vector<User> FriendModel::query(int userid)
{
    vector<User> vec;
    char sql[1024] = {0};
    // c_str()将string风格转化为c语言的char*风格
    sprintf(sql, "select a.id,a.name,a.state from user a inner join friend b on b.friendid=a.id where b.userid=%d",userid);
    MySQL mysql;
    if(mysql.connect())
    {
        MYSQL_RES * res = mysql.query(sql);
        if( res != nullptr)
        {
            MYSQL_ROW row;
            while( (row = mysql_fetch_row(res)) != nullptr )
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            }
            mysql_free_result(res);
        }
    }
    return vec;
}