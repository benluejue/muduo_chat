#ifndef GROUP_MODEL_H
#define GROUP_MODEL_H
#include "group.hpp"
#include <string>
#include <vector>
using namespace std;
class GroupModel
{
public:
    // 创建群组
    bool creatGroup(Group &group);
    // 加入群组
    void addGroup(int userid, int groupid, string role);
    // 查询用户所在群组的信息
    vector<Group> queryGroups(int userid);
    // 根据groupid查询组用户id列表，处理userid， 目的是用这些给其他用户群发消息
    vector<int> queryGroupUser(int userid, int groupid);
};
#endif