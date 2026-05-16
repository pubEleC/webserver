#include "myutils/common.h"
#include "webserver.h"
using namespace muduo;
using namespace muduo::net;
using namespace std::placeholders;

void server(int port){

    EventLoop loop;
    InetAddress addr(port);
    WebServer server(&loop,addr,"web");

    server.start();
    loop.loop();

    //telnet 127.0.0.1 22001
    //ctrl + ]
    //quit
}

void client(int port){

}

int main(int argc,char* argv[]){

    if(argc != 2){
        std::cout << "arg wrong" << '\n';
    }

    int flag = atoi(argv[1]);
    const int port = 22101;
    if(flag == 1){
        server(port);
    }
    else{
        client(port);
    }

}
