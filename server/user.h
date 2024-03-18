#ifndef __user_H__
#define __user_H__
#include<string>
using std::string;

class User{
public:
    User(int id = -1,string name ="",string pwd = "",string state = "")
    :_id(id),
    _name(name),
    _password(pwd),
    _state(state)
    {

    }
    void setId(int id){this->_id = id;}
    void setName(string name){this->_name = name;}
    void setPwd(string pwd){this->_password = pwd;}
    void setState(string state){this->_state = state;}

    int getId(){return this->_id;}
    string getName(){return this->_name;}
    string getPwd(){return this->_password;}
    string getState(){return this->_state;}
    
private:
    int _id;
    string _name;
    string _password;
    string _state;

};





#endif
