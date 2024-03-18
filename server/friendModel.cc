#include"friendModel.h"
#include"DB.h"
bool friendModel::addFriend(int userId,int friendId){
    char sql[1024] = {0};
    sprintf(sql,"insert into friend(userid,friendid) value(%d,%d)",userId,friendId);
    MySQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            return true;
        }
    }
    return false;
}
bool friendModel::removeFriend(int userId,int friendId){
    char sql[1024] = {0};
    sprintf(sql,"delete from friend where userid = %d and friendid = %d",userId,friendId);
    MySQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            return true;
        }
    }
    return false;

}
vector<User> friendModel::query(int userId){
    char sql[1024] = {0};
    sprintf(sql,"select a.id,a.name,a.state from user a inner join friend b on b.friendid = a.id where b.userid = %d",userId);
    vector<User> vec_friends;
    MySQL mysql;
    if(mysql.connect()){
       MYSQL_RES * res =  mysql.query(sql);
       MYSQL_ROW row;
       if(res != nullptr){
           while((row = mysql_fetch_row(res))!=nullptr){
               User user;
               user.setId(atoi(row[0]));
               user.setName(row[1]);
               user.setState(row[2]);
               vec_friends.push_back(user);
           }
       }
       mysql_free_result(res);
       return vec_friends;
    }
    return vec_friends;
}
