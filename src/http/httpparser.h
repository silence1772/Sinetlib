#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include "httprequest.h"

class IOBuffer;

class HttpParser
{
public:
    // 解析的各种状态
    enum ParseState
    {
        // 解析请求行
        PARSE_STATE_REQUESTLINE,
        // 解析头部字段
        PARSE_STATE_HEADER,
        // 解析主体
        PARSE_STATE_BODY,
        // 解析完成
        PARSE_STATE_GOTALL
    };

    // 解析请求行的各种状态
    enum RequestLineState
    {
        // 解析方法头
        REQUEST_LINE_STATE_METHOD,
        // 解析url
        REQUEST_LINE_STATE_URL,
        // 解析http版本
        REQUEST_LINE_STATE_VERSION,
        // 解析完成
        REQUEST_LINE_STATE_FINISH
    };

    HttpParser() :
        state_(PARSE_STATE_REQUESTLINE),
        request_line_state_(REQUEST_LINE_STATE_METHOD),
        body_length_(0) {}
    
    bool ParseRequest(IOBuffer* buf, Timestamp receive_time);

    bool IsGotAll() const { return state_ == PARSE_STATE_GOTALL; }

    // 重置
    void Reset()
    {
        state_ = PARSE_STATE_REQUESTLINE;
        request_line_state_ = REQUEST_LINE_STATE_METHOD;
        HttpRequest dummy;
        request_.Swap(dummy);
    }

    const HttpRequest& GetRequest() const { return request_; }
    
private:
    bool ParseRequestLine(const char* begin, const char* end);

    // 主解析状态
    ParseState state_;
    // 解析请求行状态
    RequestLineState request_line_state_;
    // 解析出来的http请求
    HttpRequest request_;

    // http请求的消息主体长度
    size_t body_length_;
};

#endif // HTTP_PARSER_H