#include "logfile.h"
#include "fileutil.h"

// 初始化成员变量，同时打开底层文件
LogFile::LogFile(const std:: string& file_name, int flush_every_n)
    : file_name_(file_name),
      flush_every_n_(flush_every_n),
      count_(0)
      //mutex_(new std::mutex())
{
    file_.reset(new AppendFile(file_name));
}

// 添加数据到file_缓冲区
void LogFile::Append(const char* str, int len)
{
    //std::lock_guard<std::mutex> lock(*mutex_);
    file_->Append(str, len);
    ++count_;
    if (count_ >= flush_every_n_)
    {
        count_ = 0;
        file_->Flush();
    }
}

// 冲刷file_缓冲区中的内容到磁盘文件
void LogFile::Flush()
{
    //std::lock_guard<std::mutex> lock(*mutex_);
    file_->Flush();
}