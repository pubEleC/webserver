#pragma once
#include<unordered_map>
#include<string>
class MprpcConfig{
public:
    void load_config_file(const std::string& config_file);

    std::string load(const std::string& key);

private:
    std::unordered_map<std::string,std::string> configMap_;

};