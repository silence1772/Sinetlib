#include "logger.h"
#include "logstream.h"
#include "asynclogger.h"
#include <pthread.h>

// ------------------------------------start
// 初始化日志等级
Logger::LogLevel InitLogLevel()
{
    if (getenv("LOG_DEBUG"))
        return Logger::DEBUG;
    else
        return Logger::INFO;
}

// 全局变量：日志等级
Logger::LogLevel g_loglevel = InitLogLevel();

// 获取全局日志等级
Logger::LogLevel Logger::GetGlobalLogLevel()
{
    return g_loglevel;
}
// --------------------------------------end


// ------------------------------------start
// 默认输出函数，输出至标准输出
void DefaultOutput(const char* msg, int len)
{
    size_t n = fwrite(msg, 1, len, stdout);
}

// 默认冲刷函数，冲刷标准输出流
void DefaultFlush()
{
    fflush(stdout);
}

static pthread_once_t once_control = PTHREAD_ONCE_INIT;
static AsyncLogger *asynclogger;

std::string Logger::log_file_name_ = "./sinetlib.log";

void OnceInit()
{
    asynclogger = new AsyncLogger(Logger::GetLogFileName());
    asynclogger->Start();
}

void AsyncOutput(const char* str, int len)
{
    pthread_once(&once_control, OnceInit);
    asynclogger->Append(str, len);
}

// 全局变量：输出函数
Logger::OutputFunc g_output = AsyncOutput;
// 全局变量：冲刷函数
Logger::FlushFunc  g_flush  = DefaultFlush;

// 设置输出函数
void Logger::SetOutput(OutputFunc out)
{
    g_output = out;
}

// 设置冲刷函数
void Logger::SetFlush(FlushFunc flush)
{
    g_flush = flush;
}
// --------------------------------------end


// ------------------------------------start
// 日志等级字符串数组，用于输出
const char* g_loglevel_name[Logger::NUM_LOG_LEVELS] =
{
    "DEBUG ",
    "INFO  ",
    "WARN  ",
    "ERROR ",
    "FATAL ",
};

// 构造函数，预输出默认信息
Logger::Logger(const char* file_name, int line, LogLevel level, const char* func_name)
    : file_name_(file_name),
      line_(line),
      level_(level)
{
    stream_ << g_loglevel_name[level_] << func_name << "():";
}

// 析构函数，输出日志流缓冲区中的数据，即用户自定义的内容
Logger::~Logger()
{
    stream_ << " - " << file_name_ << ':' << line_ << '\n';

    g_output(GetStream().GetBuffer().GetData(), GetStream().GetBuffer().GetLength());
    // 输出日志等级为致命时，冲刷输出流并终止程序
    if (level_ == FATAL)
    {
        g_flush();
        abort();
    }
}
// --------------------------------------end