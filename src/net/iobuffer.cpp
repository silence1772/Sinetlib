#include "iobuffer.h"
#include <errno.h>
#include <sys/uio.h>
#include <unistd.h>

const size_t IOBuffer::PREPEND_SIZE;
const size_t IOBuffer::INITIAL_SIZE;
const char   IOBuffer::CRLF[] = "\r\n";

// 读取fd上的数据
ssize_t IOBuffer::ReadFd(int fd, int* saved_errno)
{
    char extra_buf[65536];
    struct iovec vec[2];
    const size_t writable = GetWritableSize();
    vec[0].iov_base = GetBegin() + write_index_;
    vec[0].iov_len = writable;
    vec[1].iov_base = extra_buf;
    vec[1].iov_len = sizeof(extra_buf);

    // 如果缓冲区的大小大于extra_buf，则不使用extra_buf
    const int iovcnt = (writable < sizeof(extra_buf)) ? 2 : 1;
    const ssize_t n = readv(fd, vec, iovcnt);
    if (n < 0)
    {
        *saved_errno = errno;
    }
    else if ((size_t)n <= writable)
    {
        write_index_ += n;
    }
    else
    {
        write_index_ = buffer_.size();
        Append(extra_buf, n - writable);
    }
    return n;
}

// 循环读取fd上的数据
// ssize_t IOBuffer::ReadFdRepeatedly(int fd, int* saved_errno)
// {
//     int n = 0;
//     int nread = 0;
//     while ((nread = read(fd, GetWritablePtr(), GetWritableSize())) > 0)
//     {
//         n += nread;
//         write_index_ += nread;
//     }
//     if (nread == -1 && errno != EAGAIN)
//     {
//         *saved_errno = errno;
//     }
//     *saved_errno = errno;
//     return n;
// }