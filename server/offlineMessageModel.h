#ifndef __offlineMessageModel_H__
#define __offlineMessageModel_H__
#include<vector>
#include<string>
using std::string;
using std::vector;
class offMsgModel{
    public:
        bool insert(int userId,string msg);
        bool remove(int userId);
        vector<string> query(int useId);
};





#endif
