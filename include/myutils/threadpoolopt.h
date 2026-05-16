#pragma once
#include<iostream>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<vector>
#include<atomic>
#include<queue>
#include<functional>
#include<unordered_map>
#include<future>
#include<memory>
#include <type_traits>
class ThreadPool{

public:
    ThreadPool(int min = 1,int max = std::thread::hardware_concurrency());
    //ThreadPool(int min = 1,int max = 10);

    ~ThreadPool();
    void addTask(std::function<void(void)> task); 

    template<typename F,typename ...Args>
    auto addTask(F&& f,Args&&...arg) -> std::future<typename std::result_of<F(Args...)>::type>{
        using rtype = typename std::result_of<F(Args...)>::type;
        //auto ptr = make_shared<packaged_task<rtype()>>(bind(forward<F>(f),forward<Args>(arg) ...));

        //shared_ptr<packaged_task<rtype()>> ptr(bind(forward<F>(f),forward<Args>(arg) ...));

        std::shared_ptr<std::packaged_task<rtype()>> ptr(
            new std::packaged_task<rtype()>(std::bind(std::forward<F>(f), std::forward<Args>(arg)...) )
        );

        std::future<rtype> res = ptr->get_future();
        {   
            std::lock_guard<std::mutex> lck(m_queueMutex);
            m_tasks.emplace([ptr](){
                (*ptr)();
            });
        }

        m_condition.notify_one();

        return res;
    }


    // template<typename F, typename... Args>
    // auto addTask(F&& f, Args&&... args) -> future<typename result_of<F(Args...)>::type>
    // {
    //     using returnType = typename result_of<F(Args...)>::type;
    //     auto task = make_shared<packaged_task<returnType()>>(
    //         bind(forward<F>(f), forward<Args>(args)...)
    //     );
    //     future<returnType> res = task->get_future();
    //     {
    //         unique_lock<mutex> lock(m_queueMutex);
    //         m_tasks.emplace([task]() { (*task)(); });
    //     }
    //     m_condition.notify_one();
    //     return res;
    // }


    // 作者: 苏丙榅
    // 链接: https://subingwen.cn/cpp/threadpool/#3-%E7%BA%BF%E7%A8%8B%E5%BC%82%E6%AD%A5
    // 来源: 爱编程的大丙
    // 著作权归作者所有。商业转载请联系作者获得授权，非商业转载请注明出处。

    int get_maxThread(){return m_maxThread;}
    int get_minThread(){return m_minThread;}
    int get_curThread(){return m_curThread;}
    int get_idleThread(){return m_idleThread;}
    int get_exitThread(){return m_exitThread;}
private:
    void manager(void);
    void worker(void);

private:
    std::thread* m_manager;
    std::vector<std::thread::id> m_workerId;

    std::unordered_map<std::thread::id,std::thread> m_workers;

    std::atomic<int> m_maxThread = 0;
    std::atomic<int> m_minThread = 0;
    
    std::atomic<int> m_curThread = 0;
    std::atomic<int> m_idleThread = 0;
    std::atomic<int> m_exitThread = 0;
    std::atomic<bool> m_stop = 0;

    std::queue<std::function<void(void)>> m_tasks;

    std::mutex m_queueMutex;
    std::mutex m_umpMutex;

    std::condition_variable m_condition;
};