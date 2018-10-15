#ifndef FILE_H
#define FILE_H

#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#include <string>

class File
{
public:
    File(std::string path);
    //~File();

    bool Exist();
    bool IsFile();
    bool IsDir();
    bool IsLink();

    // time_t GetCreateTime();
    // time_t GetModifyTime();
    // time_t GetAccessTime();

    std::string GetAbsolutePath();
    std::string GetName();
    std::string GetExt();
    std::string GetMimeType();
    off_t GetSize();

    std::string ReadAsText();
    std::vector<File> ListDir();

private:
    struct stat file_stat_;
    bool exist_;
    std::string path_;
};

#endif // FILE_H