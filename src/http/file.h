#ifndef FILE_H
#define FILE_H

#include "timestamp.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#include <string>
#include <unordered_map>

class IOBuffer;

// 负责查看文件或目录信息
// 并通过内存映射把文件读出到指定的buffer上
class File
{
public:
    // 要求传入完整的路径
    File(std::string path);

    // 文件或目录是否存在
    bool Exist();
    // 是否为文件
    bool IsFile();
    // 是否为目录
    bool IsDir();
    // 是否为链接
    bool IsLink();

    // 获取创建时间
    Timestamp GetCreateTime();
    // 获取修改时间
    Timestamp GetModifyTime();

    // 获取文件名或目录名
    std::string GetName();
    // 获取文件格式，如为目录返回“”
    std::string GetExt();
    // 获取文件对应的http协议文件类型
    std::string GetMimeType();
    // 获取文件大小
    off_t GetSize();

    // 将文件读取为string
    std::string ReadAsText();
    // 获取目录下内容
    std::vector<File> ListDir();

    // 将文件内容读出到指定的buffer中
    void ReadToBuffer(IOBuffer& buffer);
private:
    // 记录文件或目录信息的结构体
    struct stat file_stat_;
    // 是否存在
    bool exist_;
    // 文件或目录的路径
    std::string path_;

    // MimeType表
    static std::unordered_map<std::string, std::string> mime_;
};

#endif // FILE_H