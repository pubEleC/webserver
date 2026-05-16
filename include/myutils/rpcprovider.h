#pragma once
#include "google/protobuf/service.h"
#include <memory>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/Buffer.h>
#include <muduo/base/Timestamp.h>

#include "google/protobuf/descriptor.h"
#include <string>

#include <unordered_map>


class RpcProvider{
public:

    void notifyService(google::protobuf::Service *service);

    void Run();

private:

    struct ServiceInfo{
        google::protobuf::Service * service_;
        std::unordered_map<std::string,const google::protobuf::MethodDescriptor*> methodMap_;
    };

    std::unordered_map<std::string,ServiceInfo> serviceMap_;

    void on_connection(const muduo::net::TcpConnectionPtr&);
    void on_message(const muduo::net::TcpConnectionPtr&,muduo::net::Buffer*,muduo::Timestamp);
    void send_rpc_response(const muduo::net::TcpConnectionPtr&,google::protobuf::Message *response);
    muduo::net::EventLoop eventLoop_;
};