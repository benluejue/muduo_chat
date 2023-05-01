#ifndef PUBLIC_H
#define PUBLIC_H
/**
 * server 和client公共文件
*/
enum EnMsgTyep{
    LOGIN_MSG = 1, // 登录消息
    LOGIN_MSG_ACK, // 登录确认消息
    REG_MSG, // 注册消息
    REG_MSG_ACK, // 注册确认消息 acknowledge character
    ONE_CHAT_MSG, // 聊天信息 
    ADD_FRIEND_MSG, //添加好友 1
    GROUP_CHAT_MSG,  //群聊信息 1
    ADD_GROUP_MSG,   // 添加群 1
    CREATE_GROUP_MSG, // 创建群
    LOGINOUT_MSG       // 退出
};
#define MSGID "msgid"
#endif