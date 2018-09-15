#include "fileutil.h"
#include <stdio.h>

// 构造函数，打开文件同时设置缓冲区
AppendFile::AppendFile(const std::string& file_name)
    : fp_(fopen(file_name.c_str(), "ae"))
{
    printf("%s\n", "111");
    setbuffer(fp_, buffer_, sizeof(buffer_));
    printf("%s\n", "222");
}

// 析构函数，关闭打开的文件
AppendFile::~AppendFile()
{
    fclose(fp_);
}

// 添加数据
void AppendFile::Append(const char* str, const size_t len)
{
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

void AppendFile::Flush()
{
    fflush(fp_);
}

// 往文件写入，非线程安全
size_t AppendFile::Write(const char* str, size_t len)
{
    // 由于单独开了一个log线程，无需考虑线程安全性，使用无锁版本fwrite函数提高性能
    return fwrite_unlocked(str, 1, len, fp_);
}