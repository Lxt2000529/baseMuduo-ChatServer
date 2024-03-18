#ifndef __Group_H__
#define __Group_H__
#include<string>
#include"GroupUser.h"
#include<vector>
using std::vector;
using std::string;
class Group{
public:
    Group(int id = -1,string name = "",string desc = "")
    :_id(id),
    _name(name),
    _desc(desc)
    {

    }
    void setId(int id){this->_id = id;}
    void setName(string name){this->_name = name;}
    void setDesc(string desc){this->_desc = desc;}

    int getId(){return this->_id;}
    string getName(){return _name;}
    string getDesc(){return _desc;}
    vector<GroupUser> & getUsers(){return this->_groupUser_vec;}
private:
    vector<GroupUser> _groupUser_vec;
    int _id;
    string _name;
    string _desc;
};





#endif
