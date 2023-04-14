#include "db.hpp"
// 初始化数据库连接
MySQL::MySQL()
{
    _conn = mysql_init(nullptr);
}
// 释放数据库连接资源
// 这里用UserModel示例，通过UserModel如何对业务层封装底层数据库的操作。代码示例如下：
MySQL::~MySQL()
{
    if (_conn != nullptr)
        mysql_close(_conn);
}
// 连接数据库
bool MySQL::connect()
{
    MYSQL *p = mysql_real_connect(_conn, server.c_str(), user.c_str(),
                                    password.c_str(), dbname.c_str(), 3306, nullptr, 0);
    if (p != nullptr)
    {
        // c和c++是ASCII，如果不设置中文显示可能出问题
        mysql_query(_conn, "set names gbk");
        LOG_INFO<<"connet mysql success!!";
    }else{
        LOG_INFO<<"connet fail!!";
    }
    return p;
}
// 更新操作
bool MySQL::update(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                    << sql << "更新失败!";
        return false;
    }
    return true;
}
// 查询操作
MYSQL_RES* MySQL::query(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                    << sql << "查询失败!";
        return nullptr;
    }
    return mysql_use_result(_conn);
}
MYSQL* MySQL::getConnection(){
    return _conn;
}