#ifndef LOGGER_H
#define LOGGER_H

#include "logstream.h"

class Logger
{
public:
    //日志等级
    enum LogLevel
    {
        DEBUG,      // 调试
        INFO,       // 信息
        WARN,       // 警告
        ERROR,      // 错误
        FATAL,      // 致命
        NUM_LOG_LEVELS,
    };

    // 构造、析构函数
    Logger(const char* file_name, int line, LogLevel level, const char* func_name);
    ~Logger();

    // 获取日志流对象
    LogStream&         GetStream() { return stream_; }
    // 获取全局日志等级，并非当前对象的等级
    static LogLevel    GetGlobalLogLevel();
    static std::string GetLogFileName() { return log_file_name_; }

    typedef void (*OutputFunc)(const char* msg, int len);
    typedef void (*FlushFunc)();
    // 设置输出函数
    static void SetOutput(OutputFunc);
    // 设置冲刷函数
    static void SetFlush(FlushFunc);

private:
    // 日志流
    LogStream stream_;
    // 日志等级
    LogLevel level_;
    // 文件名
    const char* file_name_; 
    // 行数
    int line_;

    static std::string log_file_name_;
};

// FIXME: 声明为内联函数或许可提高性能
// extern Logger::LogLevel g_loglevel;
// inline Logger::LogLevel Logger::GetLogLevel()
// {
//     return g_loglevel;
// }

// 宏定义，在代码中include该头文件即可使用： LOG_INFO << "输出数据";
#define LOG_DEBUG if (Logger::GetGlobalLogLevel() == Logger::DEBUG) Logger(__FILE__, __LINE__, Logger::DEBUG, __func__).GetStream()
#define LOG_INFO  if (Logger::GetGlobalLogLevel() <= Logger::INFO)  Logger(__FILE__, __LINE__, Logger::INFO, __func__).GetStream()
#define LOG_WARN  Logger(__FILE__, __LINE__, Logger::WARN, __func__).GetStream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR, __func__).GetStream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::FATAL, __func__).GetStream()

#endif // LOGGER_H