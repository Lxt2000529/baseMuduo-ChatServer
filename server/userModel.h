#ifndef __userModel_H__
#define __userModel_H__
#include"user.h"
#include"DB.h"
class userModel{
    public:
        bool insert(User& user);
        User query(int id);
        bool updateState(User &user);
        void reset();
};





#endif
