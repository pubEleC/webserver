#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/Buffer.h>
#include <muduo/base/Timestamp.h>

#include <iostream>
#include <string>
#include <functional>
#include <memory>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include <unordered_map>

using namespace muduo;
using namespace muduo::net;

class WebServer{

public:
    WebServer(EventLoop* loop,const InetAddress& listenAddr,const std::string& nameArg);
    
    void start(){server_.start();}

private:

    void onConnection(const TcpConnectionPtr &conn);

    void onMessage(const TcpConnectionPtr& conn,Buffer *buffer,muduo::Timestamp time);

    static void handleServerExit(int sig);

    TcpServer server_;
    EventLoop *loop_;

    //std::unordered_map<TcpConnectionPtr,std::string> ump;

};