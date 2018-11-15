#include "fileutil.h"
#include <stdio.h>
#include <string> // for strerror()

// 构造函数，打开文件同时设置缓冲区
AppendFile::AppendFile(const std::string& file_name)
{
    if ((fp_ = fopen(file_name.c_str(), "ae")) == NULL) // 'e' for O_CLOEXEC
    {
        printf("log file open failed: errno = %d reason = %s \n", errno, strerror(errno));
    }
    setbuffer(fp_, buffer_, sizeof(buffer_));
}

// 析构函数，关闭打开的文件
AppendFile::~AppendFile()
{
    fclose(fp_);
}

// 添加数据到缓冲区
void AppendFile::Append(const char* str, const size_t len)
{
    // 数据可能没法一次写入，故需加以解决
    size_t n = Write(str, len);
    size_t remain = len - n;
    while (remain > 0)
    {
        size_t tmp = Write(str + n, remain);
        if (tmp == 0)
        {
            int err = ferror(fp_);
            if (err)
                fprintf(stderr, "AppendFile::Append() failed !\n");
            break;
        }
        n += tmp;
        remain = len - n;
    }
}

// 冲刷缓冲区的内容写入磁盘文件
void AppendFile::Flush()
{
    fflush(fp_);
}

// 实际添加数据到缓冲区，非线程安全
size_t AppendFile::Write(const char* str, size_t len)
{
    // 由于单独开了一个log线程，无需考虑线程安全性，使用无锁版本fwrite函数提高性能
    // 该函数实际上是先添加数据到缓冲区，待到缓冲区填满或者fflush()被调用才实际写入磁盘文件
    return fwrite_unlocked(str, 1, len, fp_);
}