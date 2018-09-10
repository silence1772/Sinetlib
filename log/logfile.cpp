#include "logfile.h"
#include "fileutil.h"

LogFile::LogFile(const std:: string& file_name, int flush_every_n)
    : file_name_(file_name),
      flush_every_n_(flush_every_n),
      count_(0)
{
    file_.reset(new AppendFile(file_name));
}

void LogFile::Append(const char* str, int len)
{
    MutexLockGuard lock(*mutex_);
    file_->Append(str, len);
    ++count_;
    if (count_ >= flush_every_n_)
    {
        count_ = 0;
        file_->Flush();
    }
}

void LogFile::Flush()
{
    MutexLockGuard lock(*mutex_);
    file_->Flush();
}