#include<iostream>
#include"chatServer.h"
#include<signal.h>
#include"DB.h"
#include"chatService.h"
using namespace std;
void resetServerHandel(int){
    chatService::getInstance()->reset();
    exit(0);
} 
int main(int argc ,char **argv){
    if(argc<3){
        cerr<<"command invalid!"<<endl;
        exit(-1);
    }
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);
    signal(SIGINT,resetServerHandel);
    EventLoop loop;
    InetAddress addr(ip,port);
    const string name = "chatserver";
    chatServer server(&loop,addr,name);
    server.start();
    loop.loop();
    return 0;
}
