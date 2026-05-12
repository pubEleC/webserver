#include "AsyncLogging.h"
#include "writeLogFile.h"

class AsyncWriteFile {
    public:
        // 通过不同的名字获取不同的日志实例（单例）
        static AsyncLogging* get_Logger(const std::string& log_name) {
            
            static std::unordered_map<std::string, AsyncLogging*> loggers;
            
            if (loggers.find(log_name) == loggers.end()) {
                loggers[log_name] = new AsyncLogging();
                AsyncLogging* asylog = loggers[log_name];

                WriteLogFile* wlf = new WriteLogFile(); 
                wlf->open(log_name);

                auto outFunc = [wlf](const char* data , size_t len){
                    wlf->write(data,len);
                };
            
                asylog->setFunc(outFunc);
                asylog->setFlush(std::bind(&WriteLogFile::flush,wlf));
                asylog->Start();

            }
            return loggers[log_name];
        }

};