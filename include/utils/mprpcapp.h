#pragma once
#include <string>
#include "mprpcconfig.h"
class MprpcApp{

public:
    
    static MprpcApp& instance();
    static void init(const std::string& file);
    static MprpcConfig& get_config();

private:
    MprpcApp(){}
    MprpcApp(const MprpcApp&) = delete;
    MprpcApp(MprpcApp&&) = delete;

    static MprpcConfig config_;  

};