#ifndef USER_H
#define  USER_H
#include <string>
using namespace std;

// 对应数据库里的用户表 ORM类
class User{
public:
    User(int id=-1,string name="", string pwd="", string state="offline"){
        this->id=id;
        this->name=name;
        this->pwd=pwd;
        this->state=state;
    }

    void setId(int id){this->id = id; }
    void setName(string name) { this->name = name;  }  
    void setPwd(string pwd) {  this->pwd = pwd;  }  
    void setState(string state) {  this->state = state;  }  

    int getId(){return id;}
    string getName() const {  return name;  }  
    string getPwd() const {  return pwd;  }  
    string getState() const { return state;  }  

    
private:
    int id;
    string name;
    string pwd;
    string state;
};
#endif