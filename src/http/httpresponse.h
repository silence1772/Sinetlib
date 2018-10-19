#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include "iobuffer.h"
#include <unordered_map>

class HttpResponse
{
public:
    // 响应状态码
    enum HttpStatusCode
    {
        UNKNOWN,
        OK = 200,
        MOVED_PERMANENTLY = 301,
        BAD_REQUEST = 400,
        NOT_FOUND = 404
    };

    explicit HttpResponse(bool close) :
        close_connection_(close),
        status_code_(UNKNOWN)
    {}

    void SetCloseConnection(bool on) { close_connection_ = on; }

    bool GetCloseConnection() const { return close_connection_; }
    
    void SetStatusCode(HttpStatusCode code) { status_code_ = code; }

    void SetStatusMessage(const std::string& message) { status_message_ = message; }

    void AddHeader(const std::string& field, const std::string& value) { headers_[field] = value; }

    void SetContentType(const std::string& content_type)
    {
        // linux下文件编码默认为utf-8,加上此句避免浏览器中文乱码
        AddHeader("Content-Type", content_type + "; charset=utf-8");
    }

    IOBuffer& GetBuffer() { return buffer_; }

    // 将响应头写入到缓冲区中
    void AppendHeaderToBuffer();
    // 将消息主体写入到缓冲区中
    void AppendBodyToBuffer(std::string& body);
    
private:
    // 短连接标识
    bool close_connection_;

    // 状态码
    HttpStatusCode status_code_;
    // 状态信息
    std::string status_message_;
    // 头部字段
    std::unordered_map<std::string, std::string> headers_;

    // 发送缓冲区，用来存放响应的内容等待发送
    IOBuffer buffer_;
};

#endif // HTTP_RESPONSE_H