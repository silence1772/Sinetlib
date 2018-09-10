#ifndef LOGFILE_H
#define LOGFILE_H

#include "fileutil.h"
#include "mutexlock.h"
#include <memory>
#include <string>

class LogFile
{
public:
    LogFile(const std::string& file_name, int flush_every_n = 1024);

    void Append(const char* str, int len);
    void Flush();
    
private:
    const std::string file_name_;
    const int flush_every_n_;

    int count_;
    std::unique_ptr<MutexLock> mutex_;
    std::unique_ptr<AppendFile> file_;
};

#endif // LOGFILE_H