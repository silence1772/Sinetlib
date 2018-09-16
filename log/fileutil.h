#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <string>

// 控制磁盘文件输出的类
class AppendFile
{
public:
    // 限定显式构造，要求传入打开的文件名
    explicit AppendFile(const std::string& file_name);
    // 析构函数，将会关闭打开的文件描述符
    ~AppendFile();
    
    // 添加数据到缓冲区
    void Append(const char* str, const size_t len);
    // 冲刷缓冲区的内容写入磁盘文件
    void Flush();

private:
    // 实际添加数据到缓冲区的函数，非线程安全
    size_t Write(const char* str, size_t len);

    // 文件描述符
    FILE* fp_;
    // 给fp_使用的缓冲区
    char buffer_[1024 * 64];
};

#endif // FILEUTIL_H