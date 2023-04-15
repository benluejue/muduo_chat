#ifndef FRIEND_MODEL
#define FRIEND_MODEL
#include "user.hpp"
#include <vector>
using namespace std;
// 维护好友信息mysql的操作借楼
class FriendModel
{
public:
    // 添加好友
    void insert(int userid, int friendid);

    // 返回用户好友列表
    vector<User> query( int userid);
};
#endif
