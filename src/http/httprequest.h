#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include "timestamp.h"
#include <map>

class HttpRequest
{
public:
    enum Method
    {
        INVALID,
        GET,
        POST,
        HEAD,
        PUT,
        DELETE
    };

    enum Version
    {
        UNKNOWN,
        HTTP10,
        HTTP11
    };

    HttpRequest() :
        method_(INVALID),
        version_(UNKNOWN) {}

    void SetVersion(Version v)
    {
        version_ = v;
    }

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

    void SetPath(const char* start, const char* end)
    {
        path_.assign(start, end);
    }

    void SetQuery(const char* start, const char* end)
    {
        query_.assign(start, end);
    }

    void SetBody(const char* start, const char* end)
    {
        body_.assign(start, end);
    }

    void SetReceiveTime(Timestamp t)
    {
        receive_time_ = t;
    }

    void AddParm(std::string key, std::string value)
    {
        parm_[key] = value;
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

    std::string GetParm(std::string key)
    {
        return parm_[key];
    }

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

    Version GetVersion() const { return version_; }
    Method GetMethod() const { return method_; }
    const std::string& GetPath() const { return path_; }
    const std::string& GetQuery() const { return query_; }
    const std::string& GetBody() const { return body_; }
    Timestamp GetReceiveTime() const { return receive_time_; }
    const std::map<std::string, std::string>& GetHeaders() const { return headers_; }

    void Swap(HttpRequest& that)
    {
        std::swap(method_, that.method_);
        std::swap(version_, that.version_);
        path_.swap(that.path_);
        query_.swap(that.query_);
        std::swap(receive_time_, that.receive_time_);
        headers_.swap(that.headers_);
        body_.swap(that.body_);
    }
private:
    Method method_;
    Version version_;
    std::string path_;
    std::string query_;
    Timestamp receive_time_;
    std::map<std::string, std::string> headers_;
    std::string body_;

    std::map<std::string, std::string> parm_;
};

#endif // HTTP_REQUEST_H