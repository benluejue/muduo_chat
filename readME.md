# 基于C++实现的集群聊天服务器
## 使用技术
+ 1 json 解析
## 实现功能
## 开发环境
+ 1 基于WSL2下的Ubuntu 20.04
+ 2 mysql version 2 8.0.32
+ 3 cmake version 3.16.3
+ 4 使用基于boost的muduo库

## mysql数据库设置
### 表设计
为了开发mysql同时要安装开发包
`sudo apt-get install libmysqlclient-dev`
user表
|字段名称|字段类型|字段说明|约束|
|--|--|--|--|
|id|int|用户id|RIMARY KEY、AUTO_INCREMENT|
|name| VARCHAR(50)| 用户名 |NOT NULL, UNIQUE|
|password| VARCHAR(50) |密码| NOT NULL|
|state| ENUM('online', 'offline')| 当前登录状态| DEFAULT 'offline'|  

Friend表
| 字段名称 | 字段类型 | 字段说明 |约束|
|--|--|--|--|
| userid   | INT      | 用户id   | NOT NULL、联合主键 |
| friendid | INT      | 好友id   | NOT NULL、联合主键 |


AllGroup表
| 字段名称   | 字段类型    | 字段说明     | 约束                        |
| ---------- | ----------- | ------------ | --------------------------- |
| id         | INT         | 组id         | PRIMARY KEY、AUTO_INCREMENT |
| groupname  | VARCHAR(50) | 组名称       | NOT NULL,UNIQUE             |
| groupdesc  | VARCHAR(200)| 组功能描述 | DEFAULT ''                  |

GroupUser表
| 字段名称 | 字段类型   | 字段说明 | 约束                |
| -------- | ---------- | -------- | ------------------- |
| groupid  | INT        | 组id     | NOT NULL、联合主键 |
| userid   | INT        | 组员id   | NOT NULL、联合主键 |
| grouprole| ENUM      | 组内角色 | DEFAULT ‘normal’    |

OfflineMessage表
| 字段名称 | 字段类型    | 字段说明           | 约束   |
| -------- | ----------- | ------------------ | ------ |
| userid   | INT         | 用户id             | NOT NULL |
| message  | VARCHAR(500)| 离线消息（存储Json字符串） | NOT NULL |
创建mysql的文件[mychat.sql](mychat.sql)

## json的使用
项目里面使用的[json](https://github.com/nlohmann/json),可以实现对json的序列化和反序列化，并无缝衔接使用c++的STL  
例子如下
```cpp
#include <iostream>
#include "../thirdparty/json.hpp"
using namespace std;
using json = nlohmann::json;
int main()
{
    /**
     * json序列化
     * 将string或者其他对象变化为json格式
     */
    json js;
    // 直接序列化一个vector容器
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(5);
    // 直接将vec序列化为list:[1,2,3,4,5],
    js["list"] = vec;
    // 直接序列化一个map容器
    map<int, string> m;
    m.insert({1, "黄山"});
    m.insert({2, "华山"});
    m.insert({3, "泰山"});
    js["path"] = m;
    cout << js["path"] << endl;
    
    /**
     * json 反序列化
     * 将接受到的字符串转化为json
     * json js2 = json::parse(jsonstr);
     */
    js["name"] = "zhang san";
    string jsonstr = js.dump();
    cout << "jsonstr:" << jsonstr << endl;
    // 模拟从网络接收到json字符串，通过json::parse函数把json字符串专程json对象
    json js2 = json::parse(jsonstr);
    // 直接取key-value
    string name = js2["name"];
    cout << "name:" << name << endl;
    // 直接反序列化vector容器 json序列转化为了vector
    vector<int> v = js2["list"];
    for (int val : v)
    {
        cout << val << " ";
    }
    cout << endl;
    // 直接反序列化map容器
    map<int, string> m2 = js2["path"];
    for (auto p : m2)
    {
        cout << p.first << " " << p.second << endl;
    }
    cout << endl;
}
```

