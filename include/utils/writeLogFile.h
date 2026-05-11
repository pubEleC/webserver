#pragma once
#include<fstream>
#include<string>
#include<unistd.h>
#include <atomic>
class WriteLogFile{

public:
    // static WriteLogFile* Instance(){
    //     static WriteLogFile* instance = new WriteLogFile();
    //     return instance;
    // }

    bool open(const std::string& filename);
    bool write(const std::string& content);
    bool write(const char* data , size_t len);

    void flush();

    WriteLogFile(int MAXSIZE = 100):
    nextCount(1)
    {

    }


    ~WriteLogFile(){
        if(isClose == 1){
            this->close();
        }
    }

public:
    
    int isClose = 1;
    int openFlag = 1;
    int closeFlag = 1;
    
private:
    int checkLen(long long length);
    void check_file_exist();
    void first_check();

    bool close();


    std::ofstream fout;
    std::string preFilename_;
    std::string postFilename_;
    std::string filename_;

    std::string dir;
    int nextCount = 1;  //下一个文件名称，例如text.txt 下一个text_1.txt
    std::ios::openmode mode = std::ios::app;

    long long maxSize = 10; //10MB
    const long long mul = 1048576;  //1024 * 1024   1kB*1kB = 1MB

    int64_t time1 = 0;

    std::atomic<int> writeCount = 0;    //可以不用原子变量，这是单线程的

};