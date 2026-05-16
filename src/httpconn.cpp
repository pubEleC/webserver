#include "httpconn.h"
#include "muduo/net/TcpConnection.h"

HttpConnection::HttpConnection(muduo::net::TcpConnection* conn)
:conn_(conn)
{

}

HttpConnection::~HttpConnection(){

}

HTTP_CODE HttpConnection::parseFromString(const std::string& buf){

}

int HttpConnection::parseFromLine(){
    
}

