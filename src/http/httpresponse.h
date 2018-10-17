#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H
#include "iobuffer.h"
#include <map>


class HttpResponse
{
public:
    enum HttpStatusCode
    {
        UNKNOWN,
        OK = 200,
        MOVED_PERMANENTLY = 301,
        BAD_REQUEST = 400,
        NOT_FOUND = 404
    };

    explicit HttpResponse(bool close) :
        status_code_(UNKNOWN),
        close_connection_(close) {}
    
    void SetStatusCode(HttpStatusCode code)
    {
        status_code_ = code;
    }

    void SetStatusMessage(const std::string& message)
    {
        status_message_ = message;
    }

    void SetCloseConnection(bool on)
    {
        close_connection_ = on;
    }

    bool GetCloseConnection() const { return close_connection_; }

    void SetContentType(const std::string& content_type)
    {
        // linux下文件编码默认为utf-8,加上此句避免浏览器中文乱码
        AddHeader("Content-Type", content_type + "; charset=utf-8");
    }

    void AddHeader(const std::string& field, const std::string& value)
    {
        headers_[field] = value;
    }

    void SetBody(const std::string& body)
    {
        body_ = body;
    }

    void AppendHeaderToBuffer();
    void AppendBodyToBuffer(std::string& body);

    IOBuffer& GetBuffer()
    {
        return buffer_;
    }
private:
    std::map<std::string, std::string> headers_;
    HttpStatusCode status_code_;
    std::string status_message_;

    bool close_connection_;
    std::string body_;

    IOBuffer buffer_;
};

#endif // HTTP_RESPONSE_H