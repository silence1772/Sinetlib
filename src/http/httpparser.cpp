#include "httpparser.h"
#include "iobuffer.h"

// 解析请求行
bool HttpParser::ParseRequestLine(const char* begin, const char* end)
{
    // 状态机需要的各个变量
    // 定义在switch语句外以避免编译器warning
    const char* start = begin;
    const char* space = std::find(start, end, ' ');

    const char* question;
    const char* ampersand;
    const char* equal;

    bool succeed;
    
    // 解析http请求的第一行，如 GET /test HTTP/1.1
    while (request_line_state_ != REQUEST_LINE_STATE_FINISH)
    {
        switch(request_line_state_)
        {
            // 解析方法头
            case REQUEST_LINE_STATE_METHOD:
                //start = begin;
                //space = std::find(start, end, ' ');
                if (space != end && request_.SetMethod(start, space))
                {
                    request_line_state_ = REQUEST_LINE_STATE_URL;
                    break;
                }
                else
                {
                    return false;
                }

            // 解析请求路径
            case REQUEST_LINE_STATE_URL:
                start = space + 1;
                space = std::find(start, end, ' ');
                if (space != end)
                {
                    // 分割路径和参数
                    question = std::find(start, space, '?');
                    if (question != space)
                    {
                        // 设置路径
                        request_.SetPath(start, question);

                        // 分割参数
                        start = question + 1;
                        while ((ampersand = std::find(start, space, '&')) != space)
                        {
                            // 取参数的名称和值
                            equal = std::find(start, ampersand, '=');
                            if (equal != ampersand)
                                request_.AddQuery(start, equal, ampersand);
                            start = ampersand + 1;
                        }
                        // 处理最后一个参数
                        if (start != space)
                        {
                            equal = std::find(start, space, '=');
                            if (equal != space)
                                request_.AddQuery(start, equal, space);
                        }
                    }
                    // 没有参数
                    else
                    {
                        request_.SetPath(start, space);
                    }
                    request_line_state_ = REQUEST_LINE_STATE_VERSION;
                    break;
                }
                else
                {
                    return false;
                }

            // 解析HTTP版本
            case REQUEST_LINE_STATE_VERSION:
                start = space + 1;
                succeed = ( end-start == 8 && std::equal(start, end-1, "HTTP/1.") );
                if (succeed)
                {
                    if (*(end-1) == '1')
                        request_.SetVersion(HttpRequest::HTTP11);
                    else if (*(end-1) == '0')
                        request_.SetVersion(HttpRequest::HTTP10);
                    else
                        return false;

                    request_line_state_ = REQUEST_LINE_STATE_FINISH;
                    break;
                }
                else
                {
                    return false;
                }

            case REQUEST_LINE_STATE_FINISH:
                break;

            default:
                return false;
        }
    }
    return true;
}

// 主状态机，解析整个请求
bool HttpParser::ParseRequest(IOBuffer* buf, Timestamp receive_time)
{
    bool has_more = true;

    const char* crlf;
    bool ok;

    const char* colon;

    const char* start;
    const char* end;

    const char* ampersand;
    const char* equal;

    while (has_more)
    {
        switch(state_)
        {
            // 解析请求行
            case PARSE_STATE_REQUESTLINE:
                crlf = buf->FindCRLF();
                if (crlf)
                {
                    ok = ParseRequestLine(buf->GetReadablePtr(), crlf);
                    if (ok)
                    {
                        request_.SetReceiveTime(receive_time);
                        buf->RetrieveUntil(crlf + 2);
                        state_ = PARSE_STATE_HEADER;
                    }
                    else
                    {
                        has_more = false;
                    }
                }
                else
                {
                    has_more = false;
                }
                break;

            // 解析头部
            case PARSE_STATE_HEADER:
                crlf = buf->FindCRLF();
                if (crlf)
                {
                    colon = std::find(buf->GetReadablePtr(), crlf, ':');
                    if (colon != crlf)
                    {
                        request_.AddHeader(buf->GetReadablePtr(), colon, crlf);
                    }
                    else
                    {
                        // 设置消息主体长度
                        if (request_.GetHeader("Content-Length").size() == 0)
                        {
                            state_ = PARSE_STATE_GOTALL;
                            has_more = false;
                        }
                        else
                        {
                            body_length_ = stoi(request_.GetHeader("Content-Length"));
                            state_ = PARSE_STATE_BODY;
                        }
                    }
                    buf->RetrieveUntil(crlf + 2);
                }
                else
                {
                    has_more = false;
                }
                break;

            // 解析消息主体
            case PARSE_STATE_BODY:
                if (buf->GetReadableSize() >= body_length_)
                {
                    start = buf->GetReadablePtr();
                    end = buf->GetReadablePtr() + body_length_;

                    // 只支持application/x-www-form-urlencoded类型表单数据
                    while ((ampersand = std::find(start, end, '&')) != end)
                    {
                        // 取参数的名称和值
                        equal = std::find(start, ampersand, '=');
                        if (equal != ampersand)
                            request_.AddQuery(start, equal, ampersand);
                        start = ampersand + 1;
                    }
                    // 处理最后一个参数
                    if (start != end)
                    {
                        equal = std::find(start, end, '=');
                        if (equal != end)
                            request_.AddQuery(start, equal, end);
                    }

                    state_ = PARSE_STATE_GOTALL;
                    has_more = false;
                }
                else
                {
                    has_more = false;
                }
                break;

            default:
                has_more = false;
                break;
        }
    }
    return ok;
}