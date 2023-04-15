#ifndef GROUP_H
#define GROUP_H
#include <string>
#include <vector>
#include "groupuser.hpp"
using namespace std;
// Group 表的ORM类  Group就是allgroup
// ORM = object relation mapping 对象关系映射 将c++的类和
class Group
{
public:
    Group(int id=-1, string groupname="", string groupdesc= "")
    {
        this->id = id;
        this->groupname = groupname;
        this->groupdesc = groupdesc;
    }
    // get函数用于获取私有成员变量的值
    int getId() const { return id; }
    string getGroupName() const { return groupname; }
    string getGroupDesc() const { return groupdesc; }
    vector<GroupUser> &getUsers()  { return users; }

    // set函数用于设置私有成员变量的值
    void setId(int id) { this->id = id; }
    void setGroupName(string groupname) { this->groupname = groupname; }
    void setGroupDesc(string groupdesc) { this->groupdesc = groupdesc; }
    
private:
    int id;
    string groupname;
    string groupdesc;
    vector<GroupUser> users;
};

#endif