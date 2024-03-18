#ifndef __chatService_H__
#define __chatService_H__
#include<muduo/net/TcpConnection.h>
#include<unordered_map>
#include<functional>
#include<mutex>
#include<nlohmann/json.hpp>
#include"public.h"
#include"chatService.h"
#include"userModel.h"
#include"offlineMessageModel.h"
#include"friendModel.h"
#include"GroupModel.h"
#include"redis.h"
using json = nlohmann::json;
using namespace muduo;
using namespace muduo::net;
using std::function;
using std::mutex;
using std::unordered_map;
using MsgHandler = function<void(const TcpConnectionPtr &con,json &js,Timestamp)>;
class chatService{
public:
    static chatService * getInstance();
    void login(const TcpConnectionPtr &con,json &js,Timestamp);
    void regist(const TcpConnectionPtr &con,json &js,Timestamp);
    void oneChat(const TcpConnectionPtr &con,json &js,Timestamp);
    void friendRQ(const TcpConnectionPtr &con,json &js,Timestamp);
    void addGroup(const TcpConnectionPtr &con,json &js,Timestamp);
    void createGroup(const TcpConnectionPtr &con,json &js,Timestamp);
    void groupChat(const TcpConnectionPtr &con,json &js,Timestamp);
    void loginOut(const TcpConnectionPtr &con,json &js,Timestamp);
    void handleRedisSubscribeMsg(int userid,string msg);
    void clientCloseExp(const TcpConnectionPtr &con);
    void reset();
    MsgHandler getMsgHandler(int msgId);
private:
    chatService();
private:
    unordered_map<int ,MsgHandler> _msgHandelMap;
    userModel _userModel;
    unordered_map<int,TcpConnectionPtr> _userConnectionMap;
    mutex _connectionMutex;
    offMsgModel _offMsgModel;
    friendModel _friendModel;
    GroupModel _groupModel;
    Redis _redis;
};





#endif
