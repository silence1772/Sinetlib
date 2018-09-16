#ifndef LOGFILE_H
#define LOGFILE_H

#include "fileutil.h"
//#include <mutex>
#include <memory>
#include <string>

// 日志文件类，管理输出的日志文件
class LogFile
{
public:
    // 构造函数，要求传入要输出的日志文件名，默认每写1024条消息就强制冲刷file_的缓冲区内容到磁盘文件中
    LogFile(const std::string& file_name, int flush_every_n = 1024);

    // 添加数据到file_缓冲区
    void Append(const char* str, int len);
    // 冲刷file_缓冲区中的内容到磁盘文件
    void Flush();
    
private:
    // 日志文件名
    const std::string file_name_;
    // 每Append()n次后把内容写到磁盘文件上
    const int flush_every_n_;
    // 计数Append()次数
    int count_;

    // 保护file_的锁
    //std::unique_ptr<std::mutex> mutex_;
    // 底层文件操作类，实际执行磁盘写入
    std::unique_ptr<AppendFile> file_;
};

#endif // LOGFILE_H