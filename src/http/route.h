#ifndef ROUTE_H
#define ROUTE_H

#include "matcher.h"
#include "httpresponse.h"
#include <vector>
#include <functional>
#include <memory>

// 子路由
class Route : public std::enable_shared_from_this<Route>
{
public:
    // 自身指针
    using RoutePtr = std::shared_ptr<Route>;
    // 处理器
    using Handler = std::function<void(const HttpRequest&, std::unordered_map<std::string, std::string>&, HttpResponse*)>;

    Route();
    ~Route();

    // 添加匹配规则
    bool AddRegexpMatcher(std::string pattern, Matcher::RegexpType regexp_type, std::string key = "");

    // 设置处理函数
    RoutePtr SetHandler(Handler handler);
    // 设置路径匹配规则
    RoutePtr SetPath(std::string pattern);
    // 设置路径前缀匹配规则
    RoutePtr SetPrefix(std::string pattern);
    // 设置方法头匹配规则
    RoutePtr SetMethod(std::string pattern);
    // 设置头部匹配规则
    RoutePtr SetHeader(std::string key, std::string value);
    // 设置参数匹配规则
    RoutePtr SetQuery(std::string key, std::string value);


    Handler GetHandler() const { return handler_; }

    bool Match(const HttpRequest& request, std::unordered_map<std::string, std::string>* match_map);
private:
    // 对应的处理函数
    Handler handler_;
    // 匹配规则集合
    std::vector<Matcher> matchers_;
};

#endif // ROUTE_H