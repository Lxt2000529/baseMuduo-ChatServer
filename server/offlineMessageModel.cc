#include"offlineMessageModel.h"
#include<mysql/mysql.h>
#include"DB.h"
bool offMsgModel::insert(int userId,string msg){
    char sql[1024] = {0};
    sprintf(sql,"insert into offlinemessage(userid,message) values(%d,'%s')",userId,msg.c_str());
    MySQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            return true;
        }
    }
    return false;
}
bool offMsgModel::remove(int userId){
    char sql[1024] = {0};
    sprintf(sql,"delete from offlinemessage where userid = %d",userId);
    MySQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            return true;
        }
    }
    return false;
}
vector<string> offMsgModel::query(int userId){
    char sql[1024] = {0};
    sprintf(sql,"select message from offlinemessage where userid = %d",userId);
    MySQL mysql;
    vector<string> vec;
    if(mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        if(res!=nullptr){
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res))!=nullptr){
                 vec.push_back(row[0]);
            }
            mysql_free_result(res);
            return vec;
        }   
     }
    return vec;
}
    

