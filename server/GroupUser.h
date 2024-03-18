#ifndef __GroupUser_H__
#define __GroupUser_H__
#include"user.h"
class GroupUser
:public User
{
public:
        void setGroupPlayer(string player){this->_groupPlayer = player;}
        string getRole(){return _groupPlayer;}

private:
        string _groupPlayer;
};





#endif
