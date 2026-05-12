/****************************************************************************************/
/*   程序：此程序是字符串操作                                            */
/*   作者：pubEleC                                                                                                           */
/****************************************************************************************/
#pragma once
#include<iostream>
// #include<unistd.h>
// #include<sys/stat.h>
#include<string>
#include<vector>

std::vector<std::string> split(const std::string& input,char delimiter,char last =' ');

std::string delLast(const std::string &input,long L,long R,char signal =' ');
