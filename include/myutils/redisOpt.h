#pragma once
#include <hiredis/hiredis.h>
#include <thread>
#include <functional>
#include <iostream>
#include <mutex>
class RedisOpt{

public:
    using MessageHandler = std::function<void(int,std::string)>;

    RedisOpt();
    ~RedisOpt();

    
    bool connect(const std::string& IP,const std::string& password,int port = 6379);
    
    bool publish(int channel,std::string message);
    
    bool subscribe(int channel);
    
    bool unsubscribe(int channel);
    
    void observer_channel_message();

    void init_notify_handler(MessageHandler fn);

    // 设置 key-value
    bool set(const std::string& key, const std::string& value);
    
    // 获取 key 对应的 value
    bool get(const std::string& key, std::string &value);
    
    bool del(const std::string& key);
private:

    bool connect1(redisContext*& context,const std::string& IP,const std::string& password,int port);

    redisContext * publish_context_;
    redisContext * subsribe_context_;
    
    redisContext * rw_context_;
    std::mutex rw_mutex_;
    MessageHandler notify_message_handler;
};