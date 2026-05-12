#pragma once
#include <semaphore.h>
#include <string>
struct _zhandle;

// enum class ZkNodeType {
//     Persistent = 0,           // 持久节点
//     Ephemeral = 1,            // 临时节点
//     EphemeralSequential = 2   // 临时顺序节点 (RPC注册最常用这个)
// };

class ZkClient
{
public:
    ZkClient();
    ~ZkClient();
    void start();
    void create(const char* path,const char* data,int datalen,int state = 0);
    std::string get_data(const char* path);

private:
    _zhandle *zhandle_;
};