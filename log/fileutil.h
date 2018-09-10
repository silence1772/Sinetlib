#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <string>

class AppendFile
{
public:
    // 限定显式构造
    explicit AppendFile(const std::string& file_name);
    ~AppendFile();
    
    void Append(const char* str, const size_t len);
    void Flush();

private:
    // 往文件写入，非线程安全
    size_t Write(const char* str, size_t len);

    // 文件描述符
    FILE* fp_;
    // 给fp_使用的缓冲区
    char buffer_[1024 * 64];
};

#endif // FILEUTIL_H