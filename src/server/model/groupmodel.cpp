#include "groupmodel.hpp"
#include "db.hpp"
// 创建群组
bool GroupModel::creatGroup(Group &group)
{
    // 业务层 不要看见数据库的代码
    char sql[1024] = {0};
    // c_str()将string风格转化为c语言的char*风格
    sprintf(sql, "insert into allgroup(groupname, groupdesc) values('%s', '%s')",
            group.getGroupName().c_str(), group.getGroupDesc().c_str());
    MySQL mysql;
    if (mysql.connect())
    {
        if( mysql.update(sql) )
        {
            // 插入主键
            group.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
        
    }
    return false;
}

 // 加入群组
void GroupModel::addGroup(int userid, int groupid, string role)
{
    char sql[1024] = {0};
    // c_str()将string风格转化为c语言的char*风格
    sprintf(sql, "insert into groupuser values(%d, %d, '%s')",
            userid, groupid, role.c_str());
    MySQL mysql;
    if (mysql.connect())
    {   
        mysql.update(sql);
    }
}
// 查询用户所在群组的信息
vector<Group> GroupModel::queryGroups(int userid)
{
    vector<Group> vec;
    // 根据userid 查到 graph的id,在根据graph id查到Group
    char sql[1024] = {0};
    // c_str()将string风格转化为c语言的char*风格
    // -- 显式内连接
    // select 字段列表 from 表1,[inner] join  表2 on 连接条件
    // 
    sprintf(sql, "select a.id,a.groupname, a.groupdesc from allgroup a inner join \
            groupuser b on a.id=b.groupid where b.userid=%d",
            userid);
    MySQL mysql;
    if (mysql.connect())
    {   
        MYSQL_RES * res = mysql.query(sql);
        if( res != nullptr )
        {
            MYSQL_ROW row;
            while( (row = mysql_fetch_row(res)) != nullptr )
            {
                Group group;
                group.setId(atoi(row[0]));
                group.setGroupName(row[1]);
                group.setGroupDesc(row[2]);
                vec.push_back(group);
            }
            mysql_free_result(res);
        }
    }

    // 查询群组的用户信息
    // 填充vector<GroupUser> users;
//     +---------+--------+-----------+
//     | groupid | userid | grouprole |
//     +---------+--------+-----------+
//     |       1 |     13 | creator   |
//     |       1 |     21 | normal    |
//     |       1 |     19 | normal    |
//     +---------+--------+-----------+

    for(Group &group : vec)
    {
        sprintf(sql, "select a.id, a.name, a.state, b.grouprole from user a inner join \
            groupuser b on b.userid=a.id where b.groupid=%d",
            group.getId());
        MYSQL_RES * res = mysql.query(sql);
        if ( res != nullptr )
        {   
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr)
            {
                GroupUser user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                user.setRole(row[3]);
                group.getUsers().push_back(user);
            }
            mysql_free_result(res);
        }
    }
    return vec;
}
// 根据groupid查询组用户id列表，处理userid， 目的是用这些给其他用户群发消息，同时记录自己的id，不给别人发
vector<int> GroupModel::queryGroupUser(int userid, int groupid)
{
    char sql[1024] = {0};
    vector<int> vec;
    sprintf(sql, "select userid from groupuser where groupid=%d and userid != %d",
            groupid, userid);
    MySQL mysql;
    if( mysql.connect() ){
        MYSQL_RES * res = mysql.query(sql);
        if ( res != nullptr )
        {   
            MYSQL_ROW row;
            while( (row = mysql_fetch_row(res)) != nullptr )
            {
                vec.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
        }
    }
    
    return vec;
}

