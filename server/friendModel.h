#ifndef __fiendModel_H__
#define __fiendModel_H__
#include<vector>
#include"user.h"
using std::vector;
class friendModel{
public:
    bool addFriend(int userId,int friendId);
    bool removeFriend(int userId,int friendId);
    vector<User> query(int userId);
};





#endif
