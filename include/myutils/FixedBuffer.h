#pragma once
#include<memory>
#include<string>
#include<cstring>
#include<algorithm>
#include "timeOpt.h"
template<size_t N>
class FixedBuffer{

public:
    FixedBuffer():
    buffer_(std::make_unique<char[]>(N)),
    length_(0)
    {

    }

    FixedBuffer(const FixedBuffer& fb) = delete;
    FixedBuffer& operator=(const FixedBuffer& fb) = delete;

    //一个存在的对象移动给不存在的
    FixedBuffer(FixedBuffer&& fb) noexcept:
    buffer_(std::move(fb.buffer_)),
    length_(fb.length_)
    {
        fb.length_ = 0;
    }

    //存在的移动给存在的，但是不是构造函数
    FixedBuffer& operator=(FixedBuffer&& fb) noexcept
    {
        if(this!= &fb){
            buffer_ = std::move(fb.buffer_);
            length_ = fb.length_;
            fb.length_ = 0;
        }
        return *this;
    }    

    ~FixedBuffer() = default;

    // void append(const char* data,size_t size){
    //     size_t avaliable = N - length_;

    //     int64_t curTime = get_now_time(timeType::us);
    //     std::string timeStr = time_to_String(curTime);
    //     timeStr = "[" + timeStr + "]";
    //     size_t timeStrLen = timeStr.size();
    //     const size_t tocopy1 = std::min(timeStrLen ,avaliable);

    //     if(tocopy1 > 0){
    //         std::memcpy(buffer_.get() + length_ , timeStr.c_str() , tocopy1);
    //         length_ += tocopy1;
    //     }

    //     avaliable = N - length_;
    //     size_t tocopy2 = std::min(size ,avaliable);

    //     if(tocopy2 > 0){
    //         std::memcpy(buffer_.get() + length_ , data , tocopy2);
    //         length_ += tocopy2;
    //     }
    // }


    void append(const char* data,size_t size){
        size_t avaliable = N - length_;

        int64_t curTime = get_now_time(timeType::us);
        std::string timeStr = time_to_String(curTime);
        timeStr = "[" + timeStr + "]";

        timeStr.append(data);

        size_t timeStrLen = timeStr.size();
        const size_t tocopy1 = std::min(timeStrLen ,avaliable - 1 );

        if(tocopy1 > 0){
            std::memcpy(buffer_.get() + length_ , timeStr.c_str() , tocopy1);
            length_ += tocopy1;
        }

        std::memcpy(buffer_.get() + length_ , "\n", 1);
        length_ += 1;
    }

    void append(const char* data){
        if(data){
            append(data,std::strlen(data));
        }
    }

    size_t getLength(){
        return length_;
    }

    const char* getData() const{
        return buffer_.get();
    }

    void clear(){
        length_ = 0;
    }

    size_t avail() const{
        return N - length_;
    }


protected:

private:
    std::unique_ptr<char[]> buffer_;
    size_t length_;
};
