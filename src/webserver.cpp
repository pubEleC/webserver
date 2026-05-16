#include "webserver.h"
#include "myutils/json.hpp"
#include "httpconn.h"
#include <any>
#include <boost/any.hpp>

using json = nlohmann::json;
using namespace std::placeholders;


WebServer::WebServer(EventLoop* loop,
    const InetAddress& listenAddr,
    const std::string& nameArg)
    :server_(loop,listenAddr,nameArg)
    ,loop_(loop)
    
{
    server_.setConnectionCallback(std::bind(&WebServer::onConnection,this,_1));
    server_.setMessageCallback(std::bind(&WebServer::onMessage,this,_1,_2,_3));
    server_.setThreadNum(2);
    //ChatService::instance();


    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    //deal function
    sa.sa_handler = WebServer::handleServerExit;
    sa.sa_flags = 0;
    //忽略这些信号的行为，因为这些信号要用writeExitLog函数处理
    sigaction(SIGINT, &sa, NULL);    // 捕获 Ctrl+C     2
    sigaction(SIGTERM, &sa, NULL);   // 捕获 kill 命令  15
    sigaction(SIGHUP, &sa, NULL);    // 捕获终端关闭     1
    sigaction(SIGQUIT, &sa, NULL);   // 捕获 Ctrl+ r/   3
    
    //httpconn_ = std::make_unique<HttpConnection>();
}

WebServer::~WebServer(){

}


void WebServer::handleServerExit(int sig){
    exit(0);
}


void WebServer::onConnection(const TcpConnectionPtr &conn){
    if(!conn->connected()){
        std::cout << conn->peerAddress().toIpPort() << "   ->   " << conn->localAddress().toIpPort() << "\toffline" <<std::endl;   
        conn->shutdown();     
    }
    else{
        std::shared_ptr<HttpConnection> httpconn(new HttpConnection(conn.get()));
        conn->setContext(httpconn);
    }

}

void WebServer::onMessage(const TcpConnectionPtr& conn,Buffer *buffer,Timestamp time){
    std::string buf = buffer->retrieveAllAsString();
    const std::shared_ptr<HttpConnection> httpconn = boost::any_cast<std::shared_ptr<HttpConnection>>(conn->getContext());
    httpconn->parseFromString(buf);

    //std::cout << "time:" << time.toFormattedString(false) << "\trecv data:" << buf << std::endl;
}