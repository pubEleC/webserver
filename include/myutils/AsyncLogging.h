#pragma once

#include "MPSCBuffer.h"
#include "FixedBuffer.h"
#include <thread>
#include <functional>
#include <unordered_map>
constexpr size_t kLargeBuffer = 4*1024*1024;

//class FixedBuffer;

class AsyncLogging{
public:
    using Buffer = FixedBuffer<kLargeBuffer>;
    using BufferVector = std::vector<Buffer>;

    using OutputFunc = std::function<void(const char*, size_t)>;
    using FlushFunc = std::function<void()>;

    // static AsyncLogging* Instance(){
    //     static AsyncLogging* instance = new AsyncLogging();
    //     return instance;
    // }

    AsyncLogging()
    :func_(nullptr),
    flush_(nullptr)
    {
    }

    ~AsyncLogging()
    {
        if(thread_.joinable()){
            Stop();
        }
    }
    AsyncLogging(const AsyncLogging& al) = delete;
    AsyncLogging& operator=(const AsyncLogging& al) = delete;


    void setFunc(OutputFunc func){
        func_ = func;
    }

    void setFlush(FlushFunc flush){
        flush_ = flush;
    }

    void Append(const char* data,size_t len){
        buffer_.Append(data,len);
    }

    void Append(const std::string& data){
        buffer_.Append(data.c_str(),data.size());
    }

    void Start(){
        thread_ = std::thread(&AsyncLogging::Run,this);
    }

    void Stop(){
        buffer_.NotifyStop();
        if(thread_.joinable()){
            thread_.join();
        }
    }

    //void writeFile


private:

    void Run();

    MPSCBuffer<kLargeBuffer> buffer_;
    std::thread thread_;
    OutputFunc func_;
    FlushFunc flush_;
};