#include "httpresponse.h"
#include <stdio.h>

void HttpResponse::AppendHeaderToBuffer()
{
    char buf[32];
    snprintf(buf, sizeof(buf), "HTTP/1.1 %d ", status_code_);

    buffer_.Append(buf);
    buffer_.Append(status_message_);
    buffer_.Append("\r\n");

    if (close_connection_)
    {
        buffer_.Append("Connection: close\r\n");
    }
    else
    {
        buffer_.Append("Connection: keep-alive\r\n");
    }

    for (auto it : headers_)
    {
        buffer_.Append(it.first);
        buffer_.Append(": ");
        buffer_.Append(it.second);
        buffer_.Append("\r\n");
    }

    buffer_.Append("\r\n");
}

void HttpResponse::AppendBodyToBuffer(std::string& body)
{
    buffer_.Append(body);
}