#include "httpresponse.h"
#include "iobuffer.h"
#include <stdio.h>

void HttpResponse::AppendToBuffer(IOBuffer* output) const
{
    char buf[32];
    snprintf(buf, sizeof(buf), "HTTP/1.1 %d ", status_code_);

    output->Append(buf);
    output->Append(status_message_);
    output->Append("\r\n");

    if (close_connection_)
    {
        output->Append("Connection: close\r\n");
    }
    else
    {
        snprintf(buf, sizeof(buf), "Content-Length: %zd\r\n", body_.size());
        output->Append(buf);
        output->Append("Connection: Keep-Alive\r\n");
    }

    for (auto it : headers_)
    {
        output->Append(it.first);
        output->Append(": ");
        output->Append(it.second);
        output->Append("\r\n");
    }

    output->Append("\r\n");
    output->Append(body_);
}