#ifndef LOGFILE_H
#define LOGFILE_H

#include "fileutil.h"
#include "mutexlock.h"
#include <memory>
#include <string>

// 日志文件类，管理输出的文件
class LogFile
{
public:
    LogFile(const std::string& file_name, int flush_every_n = 1024);

    void Append(const char* str, int len);
    void Flush();
    
private:
    // 日志文件名
    const std::string file_name_;
    // 每Append()n次后把内容写到文件上
    const int flush_every_n_;
    // 计数Append()次数
    int count_;

    // 保护file_的锁
    std::unique_ptr<MutexLock> mutex_;
    // 底层文件操作类，实际执行磁盘写入
    std::unique_ptr<AppendFile> file_;
};

#endif // LOGFILE_H