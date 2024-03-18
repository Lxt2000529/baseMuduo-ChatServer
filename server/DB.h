#ifndef __DB_H__
#define __DB_H__
#include<mysql/mysql.h>
#include<string>
#include<iostream>
using std::cout;
using std::endl;
using std::string;
static string server = "127.0.0.1";
static string user = "yu";
static string password = "123456";
static string dbname = "chat";

class MySQL{
public:
    MySQL(){
        _sqlCon = mysql_init(nullptr);
    }
    ~MySQL(){
        if(_sqlCon){
            mysql_close(_sqlCon);      
        }
    }
    bool connect(){
        MYSQL *p = mysql_real_connect(_sqlCon,server.c_str(),user.c_str(),password.c_str(),dbname.c_str(),3306,nullptr,0);
        if(p!=nullptr){
            mysql_query(_sqlCon,"set names gbk");
        }else{
        }
        return p;
    }
    bool update(string sql){
        if(mysql_query(_sqlCon,sql.c_str())){
            cout<<"sql------>"<<sql<<"<------- failed"<<endl;
            return false;
        }
        cout<<"<-------update success------>"<<sql<<endl;
        return true;
    }
    MYSQL_RES * query(string sql){
        if(mysql_query(_sqlCon,sql.c_str())){
            cout<<"sql------>"<<sql<<"<------- failed"<<endl;
            return nullptr;
        }
        return mysql_use_result(_sqlCon);
    }
    MYSQL* getConnection(){
        return _sqlCon;
    }
private:
    MYSQL* _sqlCon;
};





#endif
