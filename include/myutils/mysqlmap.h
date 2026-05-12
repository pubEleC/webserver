#pragma once
#include "mysqlOpt.h"
#include <unordered_map>
#include <string>
class MysqlConnectPoolMap {
public:
    // 通过不同的名字获取不同的日志实例（单例）
    static MysqlConnectPool* get_pool(const std::string& dbName) {
        
        static std::unordered_map<std::string, MysqlConnectPool*> poolMap;
        
        if (poolMap.find(dbName) == poolMap.end()) {
            return nullptr;
        }
        return poolMap[dbName];
    }

    static void insertPool(const std::string& dbname, mysqlpool* pool) {
        poolMap[dbname] = pool_ptr; 
    }
    
private:
    MysqlConnectPoolMap() = delete;
    
    // 🌟 核心技巧：唯一的静态局部变量，被封装在一个私有静态函数里
    static std::unordered_map<std::string, std::unique_ptr<mysqlpool>>& getMap() {
        static std::unordered_map<std::string,MysqlConnectPool*> poolMap;
        return poolMap; // 返回它的引用
    }
};