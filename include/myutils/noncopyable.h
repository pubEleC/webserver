#pragma once

class noncopyable{

//这里public private都可以
public:
    noncopyable(const noncopyable&) = delete;
    void operator=(const noncopyable&) = delete;
    //连续赋值
    //noncopyable& operator=(const noncopyable&) = delete;
//派生类可以用，其他不能构造
protected:
    noncopyable() = default;
    ~noncopyable() = default;
};