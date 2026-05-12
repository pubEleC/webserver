#pragma once
#include<iostream>
#include<string>
#include <mysql/mysql.h>
#include <queue>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <thread>       // for std::this_thread::sleep_for
#include <chrono>       // for std::chrono::seconds
//#include <pthread.h>    // for pthread
#include <unistd.h> 
#include <string_view> 
#include <cstring>      //memset
#include <type_traits>   //is_same_v
#include <array>
typedef const std::string& StringRef;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;
using std::chrono::steady_clock;
using std::chrono::nanoseconds;


class MysqlConnect{

private:

    MYSQL_RES* m_res = nullptr;
    MYSQL_STMT* stmt = nullptr;
    int lastId_ = -1;

private:
    MYSQL* m_conn = nullptr;
    MYSQL_ROW m_row = nullptr;
    uint64_t m_queryRow = 0;
    unsigned int m_column;
    steady_clock::time_point m_alivetime;


    unsigned long param_count;
    std::vector<MYSQL_BIND> binds;
    std::vector<unsigned long> binds_length;

    void freeRes();
public:

    MysqlConnect();

    ~MysqlConnect();

    bool connect(StringRef user,StringRef passwd,StringRef dbName,StringRef ip,unsigned short port = 3306);

    bool update(StringRef sql);

    bool query(StringRef sql);

    bool next();

    std::string value(int index);

    bool transaction();

    bool commit();

    bool rollback();

    void refreshAliveTime();

    long long getAliveTime();

    unsigned int getColumn(){
        return m_column;
    }

    uint64_t getQueryRow(){
        return m_queryRow;
    }


    template<typename T>
void bindOne(MYSQL_BIND& bind, const T& value) {
    // 清空结构体，这是个好习惯
    // memset(&bind, 0, sizeof(bind));
    bind = {};
    // 编译期类型判断
    if constexpr (std::is_same_v<T, int>) {
        bind.buffer_type = MYSQL_TYPE_LONG;
        bind.buffer = const_cast<void*>(static_cast<const void*>(&value));
        // is_unsigned 默认为 false，无需设置
    } 
    else if constexpr (std::is_same_v<T, unsigned int>) {
        bind.buffer_type = MYSQL_TYPE_LONG;
        bind.buffer = const_cast<void*>(static_cast<const void*>(&value));
        bind.is_unsigned = true; // 区分有符号和无符号
    }
    else if constexpr (std::is_same_v<T, long long>) {
        bind.buffer_type = MYSQL_TYPE_LONGLONG;
        bind.buffer = const_cast<void*>(static_cast<const void*>(&value));
    }
    else if constexpr (std::is_same_v<T, double>) {
        bind.buffer_type = MYSQL_TYPE_DOUBLE;
        bind.buffer = const_cast<void*>(static_cast<const void*>(&value));
    }
    // 处理字符串类型
    else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
        bind.buffer_type = MYSQL_TYPE_STRING;
        bind.buffer = const_cast<void*>(static_cast<const void*>(value.data()));
        //bind.length = const_cast<unsigned long*>(static_cast<const unsigned long*>(&value.length()));
        this->binds_length.push_back(value.length());
        bind.length = const_cast<unsigned long*>(static_cast<const unsigned long*>(&binds_length.back()));
    }
    // 处理 C 风格字符串
    else if constexpr (std::is_same_v<T, const char*>) {
        bind.buffer_type = MYSQL_TYPE_STRING;
        bind.buffer = const_cast<void*>(static_cast<const void*>(value));
        // C 风格字符串需要用 strlen 计算长度，但这要求 value 必须以 '\0' 结尾
        // 注意：strlen 是运行时计算，所以我们需要一个地方存储这个长度
        // 这里为了简化，我们假设 length 字段在调用前会被正确设置，或者使用更复杂的方案
        // 一个常见的做法是让调用者传入 std::string 或 std::string_view
        //static unsigned long len = strlen(value);
        //bind.length = &len;
        //bind.length = &bindlength;
        this->binds_length.push_back(value.length());
        bind.length = const_cast<unsigned long*>(static_cast<const unsigned long*>(&binds_length.back()));
    }
    // ... 可以继续添加其他类型的支持 ...
    else {
        // 如果遇到不支持的类型，编译时报错
        static_assert(sizeof(T) == 0, "Unsupported type for bindOne!");
    }
}

    template<typename ...Args>
    bool queryStmt(StringRef sql,Args&& ...args){
        //std::cout << "free stmt1" << std::endl;

        if (stmt != nullptr) {
            if (m_conn == nullptr) {
                std::cerr << "MySQL connection is null!" << std::endl;
                return false;
            }
            //std::cout << "free stmt2" << std::endl;
            mysql_stmt_free_result(stmt);
            mysql_stmt_close(stmt);
            stmt = nullptr;
            //std::cout << "free stmt3" << std::endl;
        }
        //std::cout << "mysql stmt" << std::endl;
        if (m_conn == nullptr) {
            std::cerr << "MySQL connection is null!" << std::endl;
            return false;
        }
        stmt = mysql_stmt_init(m_conn);


        if (!stmt) {
            std::cout << "mysql_stmt_init() failed" << std::endl;
            return false;
        }
        if (mysql_stmt_prepare(stmt, sql.c_str(), sql.length())) {
            std::cout << "mysql_stmt_prepare() failed: " + std::string(mysql_stmt_error(stmt)) << std::endl;
            mysql_stmt_close(stmt);
            return false;
        }

        // if (mysql_ping(m_conn) != 0) {
        //     std::cout << "Failed to ping MySQL server, connection lost." << std::endl;

        //     std::cout << mysql_error(m_conn) << std::endl;
        //     return false;
        // }
        

        param_count = mysql_stmt_param_count(stmt);
        //binds.resize(param_count);
        binds.clear();
        binds_length.clear();

        // 1. 检查参数数量
        if (sizeof...(args) != param_count) {
            std::cout << "Parameter count mismatch. Expected: " + std::to_string(param_count) + ", Got: " + std::to_string(sizeof...(args)) << std::endl;
            return false;
        }

        // 2. 绑定参数（只在第一次调用时执行，或者如果参数类型变了才需要重新绑定，但为简单起见，我们每次都绑定）
        // 注意：这里的绑定是绑定到参数的地址，而不是值
        //bindAll(std::forward<Args>(args)...);

        binds.reserve(sizeof...(Args)); // 预分配内存，避免多次拷贝
        binds_length.reserve(sizeof...(Args));
        // 2. 定义一个 lambda，用于处理单个参数
        //    它会捕获 binds 的引用，以便向其中添加元素
        auto binder = [this](const auto& arg) {
            // 在 vector 末尾创建一个新的 MYSQL_BIND 元素
            this->binds.emplace_back(); 
            // 获取刚刚创建的元素的引用
            MYSQL_BIND& newBind = this->binds.back();
            // 调用 bindOne 来填充它
            this->bindOne(newBind, arg);
        };

        // 3. 使用折叠表达式，将 binder 应用到所有参数上
        //    (binder(args), ...) 的意思是：
        //    (binder(arg1), binder(arg2), binder(arg3), ...)
        //    逗号操作符会从左到右依次执行每个表达式
        (binder(std::forward<Args>(args)), ...);

        if (mysql_stmt_bind_param(stmt, binds.data())) {
            std::cout << "mysql_stmt_bind_param() failed: " + std::string(mysql_stmt_error(stmt)) << std::endl;
            return false;
        }

        // 3. 执行查询
        if (mysql_stmt_execute(stmt)) {
            std::cout << "mysql_stmt_execute() failed: " + std::string(mysql_stmt_error(stmt)) << std::endl;
            return false;
        }
        else{
            //std::cout << "mysql_stmt_execute() true " << std::endl;
        }

        // 4. 获取结果
        m_res = mysql_stmt_result_metadata(stmt);


        if ( mysql_stmt_field_count(stmt) > 0) {
            if(!m_res){
                std::cout << "mysql_stmt_result_metadata() failed, but query should return results: " + std::string(mysql_stmt_error(stmt)) << std::endl;
                return false;
            }
        }
        else{
            //std::cout << "已经查到数据" << std::endl;
        }

        // if(m_res != nullptr){
        //     m_queryRow = mysql_num_rows(m_res);
        // }
        // else{
        //     m_queryRow = 0;
        // }
        try{
            lastId_ = mysql_stmt_insert_id(stmt);
        }catch (const std::exception& e) {
            // 捕获其他所有可能的异常
            std::cout << "Error processing request: " << e.what() << std::endl;
        }
        return true;
    }

    //mysql_stmt_get_strstmt()

    
    template<typename T>
    void bindResOne(MYSQL_BIND& bind, T& value) 
    {
        //memset(&bind, 0, sizeof(MYSQL_BIND));
        //bind = {};
        // 使用 is_same_v 来判断
        if constexpr (std::is_same_v<T, int>) {
            //std::cout << "set int" << std::endl;
            bind.buffer_type = MYSQL_TYPE_LONG;
            bind.buffer = (char*)(&value);
        } 
        else if constexpr (std::is_same_v<T, double>){
            bind.buffer_type = MYSQL_TYPE_DOUBLE;
            bind.buffer = (char*)(&value);
        } 
        // 关键部分：判断 T 是否是字符数组类型
        else if constexpr (std::is_array_v<T> && std::is_same_v<std::remove_extent_t<T>, char>){
            // std::remove_extent_t<T> 会去掉数组的一维，例如 char[50] -> char
            //std::cout << "set string" << std::endl;
            bind.buffer_type = MYSQL_TYPE_STRING;
            bind.buffer = value; // value 会自动退化为 char*，这正是我们需要的
            bind.buffer_length = sizeof(value); // sizeof(value) 可以直接得到整个数组的大小
        }
        else if constexpr (std::is_same_v<T, MYSQL_TIME>) {
            // 告诉 MySQL 这是一个日期时间类型
            bind.buffer_type = MYSQL_TYPE_DATETIME; // 如果是纯日期用 MYSQL_TYPE_DATE
            // 把 MYSQL_TIME 结构体的地址给它
            bind.buffer = (char*)(&value);
            bind.buffer_length = sizeof(MYSQL_TIME);
        }
    }


    template<typename ...Args>
    bool bindStmt(Args&& ...args){

        //memset(bind, 0, sizeof(bind));
        std::array<MYSQL_BIND, sizeof...(Args)> bind; 
        int next_index = 0;

        // Lambda 接收当前索引，并返回下一个索引
        auto binder = [this,&bind](int current_index, auto&& arg) {
            this->bindResOne(bind[current_index], arg);
            //std::cout << "current_index: " <<current_index << std::endl;
            return current_index + 1; // 返回递增后的索引
        };
    
        // 使用逗号运算符和折叠表达式来传递索引
        // 1. binder(next_index, arg) 被调用，并返回 next_index + 1
        // 2. 逗号运算符丢弃 binder 的返回值，但我们把它赋值给 next_index
        // 3. 这个过程对每个参数重复
        ((next_index = binder(next_index, std::forward<Args>(args))), ...);
    
        
        return !mysql_stmt_bind_result(stmt, bind.data());
    }


    int nextStmt(){
        // if(stmt != nullptr){
        //     std::cout << "stmt != nullptr " << std::endl;
        // }
        return mysql_stmt_fetch(stmt);
    }


    int getLastInsertId(){
        return lastId_;
    }
};


 
class MysqlConnectPool{
public:
//static MysqlConnectPool* Instance();
    MysqlConnectPool(const std::string& ip,const std::string& user,const std::string& passwd,unsigned short port = 3306);
    ~MysqlConnectPool();
    

    MysqlConnectPool(const MysqlConnectPool& obj) = delete;
    MysqlConnectPool& operator=(const MysqlConnectPool& obj) = delete;

    int getFlag(){
        return flag;
    }

    void setDbName(const std::string& dbName);
    // void setDbName(const std::string& dbName){
    //     m_dbName = dbName;
    // }

    std::shared_ptr<MysqlConnect> getMysqlConnect();
    //MysqlConnect* getMysqlConnect();
private:

    void produceConnect();
    void recycleConnect();

    std::string m_ip;
    std::string m_user;
    std::string m_passwd;
    std::string m_dbName;
    unsigned short m_port = 3306;

public:
    //第一次生产的值
    int m_minSize = 30;
    //小于该值，则生产
    int m_minSizePro = 25;
    //大于该值，则回收
    int m_minSizeRe = 100;
    int m_maxSize = 1024;
    int m_maxIdleTime = 10000000;
    int m_timeout = 1000;
    std::queue<MysqlConnect*> q_connect;
    //pthread_mutex_t q_mutex;
    //pthread_cond_t q_cond;
    //pthread_cond_t get_cond;
    std::mutex q_mutex;
    std::condition_variable q_cond;
    std::condition_variable get_cond;
    int flag = 0;
    void addConnect();
    //void addConnect(MysqlConnect* conn);
};


//void * produceConnect(void * arg);
//void * recycleConnect(void * arg);