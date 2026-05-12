#pragma once
#include <google/protobuf/service.h>
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "userservice.h"
#include "rpchearder.pb.h"
#include "mprpcapp.h"
#include "zkopt.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <error.h>
class MprpcChannel : public google::protobuf::RpcChannel
{
public:
    void CallMethod(const google::protobuf::MethodDescriptor* method,
        google::protobuf::RpcController* controller, const google::protobuf::Message* request,
        google::protobuf::Message* response, google::protobuf::Closure* done)
    {
        const google::protobuf::ServiceDescriptor* serviceDesc = method->service();
        std::string serviceName = serviceDesc->name();
        std::string methodName = method->name();

        //[4字节长度len][服务名，方法名，参数长度len2][参数值]
        //len rpcHeaderStr.size() argsStr.size()
        std::string argsStr;
        if(!request->SerializeToString(&argsStr)){
            controller->SetFailed("1");
            LOG_INFO("serialize request argsStr error %s", argsStr.c_str());
        }

        mprpc::RpcHeader rpcHeader;
        rpcHeader.set_service_name(serviceName);
        rpcHeader.set_method_name(methodName);
        
        rpcHeader.set_args_size(argsStr.size());


        std::string rpcHeaderStr;
        
        
        if(!rpcHeader.SerializeToString(&rpcHeaderStr)){
            controller->SetFailed("1");
            LOG_INFO("serialize request rpcHeaderStr error %s", rpcHeaderStr.c_str());
        }
        int32_t rpcHeaderStrLenHost = rpcHeaderStr.size();
        //int32_t rpcHeaderStrLen = htonl(rpcHeaderStr.size());
        int32_t rpcHeaderStrLen = htonl(rpcHeaderStrLenHost);
        //std::string rpcHeaderLenStr = reinterpret_cast<const char*>(&rpcHeaderStrLen);

        std::string rpcHeaderLenStr(4,'0');
        memcpy(&rpcHeaderLenStr[0],(char*)&rpcHeaderStrLen,4);

        // const char* ch1 = &rpcHeaderLenStr[0];
        // for(int i = 0;i<4;i++){
        //     int32_t temp = static_cast<int32_t>(*ch1);
        //     ch1++;
        //     LOG_INFO("%d",temp);
        // }

        std::string sendRpcStr;
        //sendRpcStr.append(std::to_string(rpcHeaderStrLen));

        sendRpcStr.append(rpcHeaderLenStr);
        sendRpcStr.append(rpcHeaderStr);
        sendRpcStr.append(argsStr);

        LOG_INFO("长度HOST:%d",rpcHeaderStrLenHost);
        //LOG_INFO("长度NET:%d",rpcHeaderStrLen);
        LOG_INFO("rpcHeaderStr:%s",rpcHeaderStr.c_str());
        LOG_INFO("argsStr:%s",argsStr.c_str());

        int clientfd = socket(AF_INET,SOCK_STREAM,0);
        if(clientfd == -1){
            controller->SetFailed("1");
            LOG_INFO("clientfd error : %d", errno);
            close(clientfd);
            exit(-1);
        }


        //std::string ip = MprpcApp::get_config().load("rpcserverip");
        //uint16_t port = atoi(MprpcApp::get_config().load("rpcserverport").c_str());
        ZkClient zk;
        zk.start();
        std::string method_path = "/" + serviceName + "/" + methodName;
        std::string net_data = zk.get_data(method_path.c_str());
        if(net_data == ""){
            controller->SetFailed("1");
            LOG_INFO("can't find zk");
            return;
        }
        
        int index = net_data.find(':');
        if(index <= 0){
            controller->SetFailed("1");
            LOG_INFO("net data error!");
            return;
        }

        // 123:12
        // 012345
        // 123   sub(0,3)
        // 12    sub(4,6-3-1)
        std::string ip = net_data.substr(0,index);
        uint16_t port = atoi(net_data.substr(index+1,net_data.size() - index - 1).c_str());

        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_addr.s_addr= inet_addr(ip.c_str());

        // LOG_INFO("ip %s",ip.c_str());
        // LOG_INFO("port: %d",port);

        if(connect(clientfd,(sockaddr*)&serverAddr,sizeof(serverAddr)) == -1){
            controller->SetFailed("1");
            LOG_INFO("connect error : %d", errno);
            close(clientfd);
            exit(-1);
        }



        if(send(clientfd,sendRpcStr.c_str(),sendRpcStr.size(),0) == -1){
            controller->SetFailed("1");
            LOG_INFO("send error : %d", errno);
            close(clientfd);
            exit(-1);
        }

        char buf[1024] = {0};
        int recvLen = recv(clientfd,buf,1024,0);
        if(recvLen == -1){
            controller->SetFailed("1");
            LOG_INFO("send error : %d", errno);       
            close(clientfd);
            exit(-1);     
        }

        std::string responseStr(buf,0,recvLen);

        if(!response->ParseFromString(responseStr)){
            controller->SetFailed("1");
            LOG_INFO("ParseFromString error : %d", errno);
            close(clientfd);
            exit(-1);
        }

        close(clientfd);

    }

};