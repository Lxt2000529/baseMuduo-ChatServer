#include"chatService.h"
#include<unistd.h>
#include<iostream>
#include<utility>
#include"user.h"
using std::cout;
using std::endl;
chatService::chatService()
{
    _msgHandelMap.insert({LOGIN_MSG,std::bind(&chatService::login,this,_1,_2,_3)});
    _msgHandelMap.insert({REGIST_MSG,std::bind(&chatService::regist,this,_1,_2,_3)});
    _msgHandelMap.insert({ONE_CHAT_MSG,std::bind(&chatService::oneChat,this,_1,_2,_3)});
    _msgHandelMap.insert({FRIEND_RQ_MSG,std::bind(&chatService::friendRQ,this,_1,_2,_3)});
    _msgHandelMap.insert({CREATE_GROUP_MSG,std::bind(&chatService::createGroup,this,_1,_2,_3)});
    _msgHandelMap.insert({ADD_GROUP_MSG,std::bind(&chatService::addGroup,this,_1,_2,_3)});
    _msgHandelMap.insert({GROUP_CHAT_MSG,std::bind(&chatService::groupChat,this,_1,_2,_3)});
    _msgHandelMap.insert({LOGIN_OUT_MSG,std::bind(&chatService::loginOut,this,_1,_2,_3)});

    if(_redis.connect()){
        _redis.init_notify_handler(std::bind(&chatService::handleRedisSubscribeMsg,this,_1,_2));
    }
}
chatService * chatService::getInstance(){
    static chatService service;
    return &service;
}
void chatService::reset(){
    _userModel.reset();

}
void chatService::oneChat(const TcpConnectionPtr &con,json &js,Timestamp){
    int to_id = js["to_id"];
    {
        std::lock_guard<mutex> lock(_connectionMutex);
        auto it = _userConnectionMap.find(to_id);
        if(it!=_userConnectionMap.end()){
            //在线
            it->second->send(js.dump());
            return;
        }
        User user = _userModel.query(to_id);
        if(user.getState() == "online"){
            _redis.publish(user.getId(),js.dump());
            return;
        }
        _offMsgModel.insert(user.getId(),js.dump());
    }
}


void chatService::login(const TcpConnectionPtr &con,json &js,Timestamp){
    int id = js["id"];
    string pwd = js["password"];
    User user = _userModel.query(id);
    if(user.getId() == id && user.getPwd()==pwd){
        if(user.getState() == "online"){
            //说明用户已经登陆
            json res;
            res["msgid"] = LOGIN_ACK;
            res["errno"] = 2;
            res["errMsg"] = "reLogin";
            con->send(res.dump());
        }else{
            user.setState("online");
            _userModel.updateState(user);

            json response;
            response["msgid"] = LOGIN_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();
            cout<<"------->login success<-------"<<endl;
            _redis.subscribe(user.getId());
            //需要加锁
            std::lock_guard<mutex> lock(_connectionMutex);
            _userConnectionMap.insert({id,con});

            //发送离线消息
            vector<string> vec_msg = _offMsgModel.query(user.getId());
            if(!vec_msg.empty()){
                response["offMsg"] = vec_msg;
                _offMsgModel.remove(user.getId());
            }
            //写入朋友信息
            vector<User> vec_friends = _friendModel.query(user.getId());
            if(!vec_friends.empty()){
                vector<string> vec2;
                for(auto &user:vec_friends){
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    vec2.push_back(js.dump());
                }
                response["friends"] = vec2;
            }
            //写入群组信息
            vector<Group> groupuserVec = _groupModel.query(id);
            if (!groupuserVec.empty())
            {
                // group:[{groupid:[xxx, xxx, xxx, xxx]}]
                vector<string> groupV;
                for (Group &group : groupuserVec)
                {
                    json grpjson;
                    grpjson["id"] = group.getId();
                    grpjson["groupname"] = group.getName();
                    grpjson["groupdesc"] = group.getDesc();
                    vector<string> userV;
                    for (GroupUser &user : group.getUsers())
                    {
                        json js;
                        js["id"] = user.getId();
                        js["name"] = user.getName();
                        js["state"] = user.getState();
                        js["role"] = user.getRole();
                        userV.push_back(js.dump());
                    }
                    grpjson["users"] = userV;
                    groupV.push_back(grpjson.dump());
                }

                response["groups"] = groupV;
            }
            con->send(response.dump());
        }
    }else{
        json response;
        response["msgid"] = LOGIN_ACK;
        response["erron"] = 1;
        con->send(response.dump());
        cout<<"------>login fail<-------"<<endl;

    }
}
void chatService::regist(const TcpConnectionPtr &con,json &js,Timestamp){
    string name = js["name"];
    string password = js["password"];
    User user;
    user.setName(name);
    user.setPwd(password);
    user.setState("offline");
    bool ret = _userModel.insert(user);
    if(ret){
        json response;
        response["msgid"] = REGIST_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        con->send(response.dump());
        cout<<"regist success..."<<endl;
    }else{
        json response;
        response["msgid"] = REGIST_ACK;
        response["errno"] = 1;
        con->send(response.dump());
        
    }
}
MsgHandler chatService::getMsgHandler(int msgId){
    auto iter = _msgHandelMap.find(msgId);
    if(iter!=_msgHandelMap.end()){
        return _msgHandelMap[msgId];
    }else{
        return [=](const TcpConnectionPtr &con,json &js,Timestamp){
            for(auto &it :_msgHandelMap){
                cout<<it.first<<endl;
            }
            cout<<"msgid: "<<msgId<<" not find ...."<<endl;
        };
    }
}
void chatService::clientCloseExp(const TcpConnectionPtr &con){
    User user;
    {
        std::lock_guard<mutex> lock(_connectionMutex);
        for(unordered_map<int,TcpConnectionPtr>::iterator mapCon = _userConnectionMap.begin();mapCon!=_userConnectionMap.end();++mapCon){
            if(mapCon->second==con){
                user.setId(mapCon->first);
                _userConnectionMap.erase(mapCon);
                break;
            }
        }
    }
    _redis.unsubscribe(user.getId());
    if(user.getId()!=-1){
        user.setState("offline");
        _userModel.updateState(user);
        cout<<"---->id: "<<user.getId()<<" offline<-----"<<endl;
    }
}
void chatService::friendRQ(const TcpConnectionPtr &con,json &js,Timestamp){
    
    cout<<"before js... "<<endl;
    int userid = js["id"].get<int>();
    int friendId = js["to_friend"].get<int>();
    cout<<"id: "<<userid<<"  friend_id: "<<friendId<<endl;
    User user = _userModel.query(friendId);
    if(user.getId() == -1){
        con->send("please check friend id ,not find ");
        return ;
    }
    _friendModel.addFriend(userid,friendId);

}

void chatService:: addGroup(const TcpConnectionPtr &con,json &js,Timestamp){
   int userid = js["userid"].get<int>();
   int groupid = js["groupid"].get<int>();
   string role = "normal";
   _groupModel.addGroup(userid,groupid,role);
}
void chatService::createGroup(const TcpConnectionPtr &con,json &js,Timestamp){
    string groupName = js["groupname"];
    string groupDesc = js["groupdesc"];
    int userid = js["userid"].get<int>();
    Group group(-1,groupName,groupDesc);
    if(_groupModel.createGroup(group)){
        _groupModel.addGroup(userid,group.getId(),"creator");
    }
}
void chatService::groupChat(const TcpConnectionPtr &con,json &js,Timestamp){
    int userid = js["userid"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> vec_users = _groupModel.queryOtherUserId(userid,groupid);
    if(!vec_users.empty()){
        for(auto &userid:vec_users){
            std::lock_guard<mutex> lock(_connectionMutex);
            if(_userConnectionMap.find(userid)!=_userConnectionMap.end()){
                _userConnectionMap[userid]->send(js.dump());
                continue;
            }
            User user = _userModel.query(userid);
            if(user.getState() == "online"){
                _redis.publish(userid,js.dump());
                continue;
            }
            _offMsgModel.insert(userid,js.dump());
        }
    }

}




void chatService::loginOut(const TcpConnectionPtr &con,json &js,Timestamp){
    int userid = js["id"].get<int>();
    {
        std::lock_guard<mutex> lock(_connectionMutex);
        auto it = _userConnectionMap.find(userid);
        if(it!=_userConnectionMap.end()){
            _userConnectionMap.erase(it);
        }
    }
    _redis.unsubscribe(userid);
    User user(userid,"","","offline");
    _userModel.updateState(user);


}
void chatService::handleRedisSubscribeMsg(int userid,string msg){
    lock_guard<mutex> lock(_connectionMutex);
    auto it = _userConnectionMap.find(userid);
    if(it!=_userConnectionMap.end()){
        it->second->send(msg);
        return;
    }
    _offMsgModel.insert(userid,msg);
}
