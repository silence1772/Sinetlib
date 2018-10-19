#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include "timestamp.h"
#include <unordered_map>

class HttpRequest
{
public:
    // 方法头
    enum Method
    {
        INVALID,
        GET,
        POST,
        HEAD,
        PUT,
        DELETE
    };

    // Http版本
    enum Version
    {
        UNKNOWN,
        HTTP10,
        HTTP11
    };

    HttpRequest() :
        method_(INVALID),
        version_(UNKNOWN) {}

    bool SetMethod(const char* start, const char* end)
    {
        std::string m(start, end);
        if (m == "GET")
            method_ = GET;
        else if (m == "POST")
            method_ = POST;
        else if (m == "HEAD")
            method_ = HEAD;
        else if (m == "PUT")
            method_ = PUT;
        else if (m == "DELETE")
            method_ = DELETE;
        else 
            method_ = INVALID;

        return method_ != INVALID;
    }

    Method GetMethod() const { return method_; }

    const char* GetMethodStr() const
    {
        const char* result = "UNKNOWN";
        switch(method_)
        {
        case GET:
            result = "GET";
            break;
        case POST:
            result = "POST";
            break;
        case HEAD:
            result = "HEAD";
            break;
        case PUT:
            result = "PUT";
            break;
        case DELETE:
            result = "DELETE";
            break;
        default:
            break;
        }
        return result;
    }

    void SetVersion(Version v) { version_ = v; }

    Version GetVersion() const { return version_; }

    void SetPath(const char* start, const char* end) { path_.assign(start, end); }

    const std::string& GetPath() const { return path_; }

    void AddQuery(const char* start, const char* equal, const char* end)
    {
        std::string field(start, equal++);
        std::string value(equal, end);
        querys_[field] = value;
    }

    const std::string GetQuery(const std::string& field) const
    {
        //return querys_[field];
        auto i = querys_.find(field);
        if (i != querys_.end())
            return i->second;
        return "";
    }

    void AddHeader(const char* start, const char* colon, const char* end)
    {
        std::string field(start, colon++);
        while (colon < end && isspace(*colon))
        {
            ++colon;
        }
        std::string value(colon, end);
        while (!value.empty() && isspace(value[value.size() - 1]))
        {
            value.resize(value.size() - 1);
        }
        headers_[field] = value;
    }

    std::string GetHeader(const std::string& field) const
    {
        std::string result;
        auto it = headers_.find(field);
        if (it != headers_.end())
        {
            result = it->second;
        }
        return result;
    }

    Timestamp GetReceiveTime() const { return receive_time_; }

    void SetReceiveTime(Timestamp t) { receive_time_ = t; }
    
    void Swap(HttpRequest& that)
    {
        std::swap(method_, that.method_);
        std::swap(version_, that.version_);
        path_.swap(that.path_);
        querys_.swap(that.querys_);
        headers_.swap(that.headers_);
        std::swap(receive_time_, that.receive_time_);
    }

private:
    // 请求方法
    Method method_;
    // Http版本
    Version version_;
    // 路径
    std::string path_;
    // 参数
    std::unordered_map<std::string, std::string> querys_;
    // 头部字段
    std::unordered_map<std::string, std::string> headers_;
    // 接收时间
    Timestamp receive_time_;
};

#endif // HTTP_REQUEST_H