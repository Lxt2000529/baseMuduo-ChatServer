#include"GroupModel.h"
#include<mysql/mysql.h>
#include"DB.h"
bool GroupModel::addGroup(int userId,int groupId,string role){
    char sql[1024] = {0};
    sprintf(sql,"insert into groupuser values(%d,%d,'%s')",groupId,userId,role.c_str());
    MySQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            return true;
        }
    }
    return false;
}
bool GroupModel::createGroup(Group &group){
    char sql[1024] = {0};
    sprintf(sql,"insert into allgroup (groupname,groupdesc) values('%s','%s')",group.getName().c_str(),group.getDesc().c_str());
    MySQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            group.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}
vector<Group> GroupModel::query(int userid){
    char sql[1024] = {0};
    sprintf(sql, "select a.id,a.groupname,a.groupdesc from allgroup a inner join groupuser b on b.groupid = a.id where b.userid = %d",userid);
    MySQL mysql;
    vector<Group> res_vec;
    if(mysql.connect()){
        MYSQL_RES * res = mysql.query(sql);
        if(res){
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res))!=nullptr){
                Group group;
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                res_vec.push_back(group);
            }
            return res_vec;
        }
    }
    return res_vec;
}
vector<int> GroupModel::queryOtherUserId(int userId,int groupId){
    char sql[1024] = {0};
    sprintf(sql, "select userid from groupuser where groupid = %d and userid !=%d",groupId,userId);
    MySQL mysql;
    vector<int> res_vec;
    if(mysql.connect()){
        MYSQL_RES * res = mysql.query(sql);
        if(res){
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res))!=nullptr){
                res_vec.push_back(atoi(row[0]));
            
            }
            return res_vec;
        }
    }
    return res_vec;
}
