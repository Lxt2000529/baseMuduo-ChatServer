#include"chatServer.h"
#include"nlohmann/json.hpp"
#include<functional>
#include<iostream>
#include"chatService.h"
using std::function;
using namespace std::placeholders;
using std::cout;
using std::endl;
using json = nlohmann::json;
using MsgHandler = function<void(const TcpConnectionPtr &con,json &js,Timestamp)>;
//EventLoop -> epoll_event epoll_wait epoll_create ...
//InetAddress -> sockaddr_in addr sin_family sin_port sin_addr.s_addr
chatServer::chatServer(EventLoop * loop,const InetAddress &addr,const string &name)
:_loop(loop),
_server(loop,addr,name)
{
    _server.setConnectionCallback(std::bind(&chatServer::onConnection,this,_1));
    _server.setMessageCallback(std::bind(&chatServer::onMsg,this,_1,_2,_3));
    _server.setThreadNum(4);
}
void chatServer::start(){
    _server.start();
}
void chatServer::onConnection(const TcpConnectionPtr &con){
    if(!con->connected()){
        chatService::getInstance()->clientCloseExp(con);
        con->shutdown();
    }
}
void chatServer::onMsg(const TcpConnectionPtr &con,Buffer *buff,Timestamp timestamp){
    string buf = buff->retrieveAllAsString();
    json js = json::parse(buf);
    auto msgHandler = chatService::getInstance()->getMsgHandler(js["msgid"].get<int>());
    if(msgHandler){
        cout<<js<<endl;
    }
    msgHandler(con,js,timestamp);
}
