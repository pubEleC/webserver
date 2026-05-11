#pragma once
#include "FixedBuffer.h"
#include <mutex>
#include <condition_variable>
#include <memory>
#include <optional>
#include <vector>
#include <chrono>
#include <atomic>

template <size_t BufferSize>
class MPSCBuffer{
public:
    using Buffer = FixedBuffer<BufferSize>;
    using BufferVector = std::vector<Buffer>;

    MPSCBuffer():
    stopped_(false)   
    {
        bufferProd_A_.emplace();
        bufferProd_B_.emplace();
        bufferCon_1_.emplace();
        bufferCon_2_.emplace();
    
        pending_Buffers_.reserve(16);
    }

    MPSCBuffer(const MPSCBuffer& mpscb) = delete;
    MPSCBuffer& operator=(const MPSCBuffer& mpscb) = delete;

    MPSCBuffer(MPSCBuffer&& mpscb) = delete;
    MPSCBuffer& operator=(MPSCBuffer&& mpscb) = delete;

    void Append(const char* data,size_t len){
        std::lock_guard<std::mutex> lock(mutex_);

        if(!bufferProd_A_.has_value()){
            bufferProd_A_.emplace();
        }

        if(bufferProd_A_->avail() > len){
            bufferProd_A_->append(data,len);
            return;
        }
        //我们要尽可能的让缓冲区写满，然后再后续刷盘，但是此时可能一直写不满，
        //所以后续消费者不能仅仅判断是否pending_Buffers为空，还需要看缓冲区是否有数据
        pending_Buffers_.push_back(std::move(*bufferProd_A_));
        bufferProd_A_.reset();


        //此时A为空，pendingBUffers 数据+1，那么后续会回收1个给A
        //如果B不为空，那么A变成B了，因为达到这个if的时候A本身就满了，那么新来的B又可以写数据
        if(bufferProd_B_.has_value()){
            bufferProd_A_ = std::move(bufferProd_B_);
            bufferProd_B_.reset();
        }
        else{
            bufferProd_A_.emplace();
        }
        bufferProd_A_->append(data,len);

        cond_.notify_one();
    }

    BufferVector WaitAndSwap(std::chrono::seconds timeout){
        BufferVector result;
        std::unique_lock<std::mutex> lock(mutex_);

        //中途停止 或者 不为空
        bool hasData = cond_.wait_for(lock,timeout,[this](){
            return stopped_.load() || !pending_Buffers_.empty();
        });

        //中途停止     empty()   A不存在 或 A存在但是没内容 ，也就是完全可以停止了
        if(stopped_.load() && pending_Buffers_.empty() && (!bufferProd_A_.has_value() || bufferProd_A_->getLength() == 0 ) ){
            return result;
        }

        //超时有数据 或者 无数据
        // if(!hasData){
        //     if(!bufferProd_A_.has_value() || bufferProd_A_->getLength() == 0){
        //         return result;
        //     }

        // }

        //不管有没有数据都应该将A的数据写入进数组
        if(bufferProd_A_.has_value() && bufferProd_A_->getLength() > 0){
            pending_Buffers_.push_back(std::move(*bufferProd_A_));
            bufferProd_A_.reset();
        }

        //此时A必为空
        if(bufferCon_1_.has_value()){
            bufferProd_A_ = std::move(bufferCon_1_);
            bufferCon_1_.reset();
        }

        result.swap(pending_Buffers_);

        if(bufferProd_B_.has_value() && bufferCon_2_.has_value()){
            bufferProd_B_ = std::move(bufferCon_2_);
            bufferCon_2_.reset();
        }     

        return result;
    }

    void Recycle(BufferVector& buffers){
        // if(buffers.size() > 2){
        //     buffers.resize(2);
        // }

        if(!bufferCon_1_.has_value() && !buffers.empty()){
            bufferCon_1_ = std::move(buffers.back());
            buffers.pop_back();
            bufferCon_1_->clear();
        }

        if(!bufferCon_2_.has_value() && !buffers.empty()){
            bufferCon_2_ = std::move(buffers.back());
            buffers.pop_back();
            bufferCon_2_->clear();
        }

        buffers.clear();

    }

    void NotifyStop(){
        stopped_.store(true);
        cond_.notify_all();
    }

    bool IsStopped() const{
        return stopped_.load();
    }

private:
    //有mutex 不可copy move
    std::mutex mutex_;
    std::condition_variable cond_;

    std::optional<Buffer> bufferProd_A_;
    std::optional<Buffer> bufferProd_B_;

    BufferVector pending_Buffers_;

    std::optional<Buffer> bufferCon_1_;
    std::optional<Buffer> bufferCon_2_;

    std::atomic<bool> stopped_;
};