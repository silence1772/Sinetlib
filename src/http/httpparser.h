#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include "httprequest.h"

class IOBuffer;

class HttpParser
{
public:
    enum ParseState
    {
        PARSE_STATE_REQUESTLINE,
        PARSE_STATE_HEADER,
        PARSE_STATE_BODY,
        PARSE_STATE_GOTALL
    };

    enum RequestLineState
    {
        REQUEST_LINE_STATE_METHOD,
        REQUEST_LINE_STATE_URL,
        REQUEST_LINE_STATE_VERSION,
        REQUEST_LINE_STATE_FINISH
    };

    HttpParser() :
        state_(PARSE_STATE_REQUESTLINE),
        request_line_state_(REQUEST_LINE_STATE_METHOD) {}
    
    //bool ParseRequest(IOBuffer* buf, Timestamp receive_time);
    bool ParseRequest(IOBuffer* buf);

    bool IsGotAll() const { return state_ == PARSE_STATE_GOTALL; }

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

    ParseState state_;
    RequestLineState request_line_state_;
    HttpRequest request_;
};

#endif // HTTP_PARSER_H