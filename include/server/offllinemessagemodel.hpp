#ifndef OFF_LINE_MESSAGE_MODEL
#define OFF_LINE_MESSAGE_MODEL
#include <string>
#include <vector>
using namespace std;
class OfflineMsg
{
public:
// 存储用户的离线消息
bool insert(int userid, string msg);
// 删除用户的离线消息
void remove(int userid);
// 查询 用户的离线消息
vector<string> query(int userid);
};
#endif