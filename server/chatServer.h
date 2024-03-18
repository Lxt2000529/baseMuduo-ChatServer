#ifndef __chatServer_H__
#define __chatServer_H__
#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
using namespace muduo;
using namespace muduo::net;
class chatServer{
public:
    chatServer(EventLoop *loop,const InetAddress &addr,const string &name);
    void start();
private:
    void onConnection(const TcpConnectionPtr&);
    void onMsg(const TcpConnectionPtr&,Buffer * buffer,Timestamp timestamp);
private:
    TcpServer _server;
    EventLoop * _loop;
};





#endif
