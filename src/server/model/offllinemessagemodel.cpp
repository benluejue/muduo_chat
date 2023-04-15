#include "offllinemessagemodel.hpp"
#include "db.hpp"
#include <iostream>

// 存储用户的离线消息
bool OfflineMsg::insert(int userid, string msg)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into offlinemessage(userid, message) values(%d,'%s')",
            userid, msg.c_str());
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
// 删除用户的离线消息
void OfflineMsg::remove(int userid)
{
    char sql[1024] = {0};
    sprintf(sql, "delete from offlinemessage where userid=%d", userid);
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}
// 查询 用户的离线消息
vector<string> OfflineMsg::query(int userid)
{
    vector<string> vec;
    char sql[1024] = {0};
    sprintf(sql, "select message from offlinemessage where userid=%d", userid);
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while( (row = mysql_fetch_row(res)) != nullptr ){
                vec.push_back(row[0]);
            }
            mysql_free_result(res);
        }
       
    }
    return vec;
}