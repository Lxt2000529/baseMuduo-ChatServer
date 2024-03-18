#ifndef __GroupModel_H__
#define __GroupModel_H__
#include"Group.h"
#include<vector>

using std::vector;
class GroupModel{
public:
    bool addGroup(int userId,int groupId,string role);
    bool createGroup(Group &group);
    vector<Group> query(int userId);
    vector<int> queryOtherUserId(int userId,int groupId);
};





#endif
