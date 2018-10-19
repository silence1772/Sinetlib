#include "route.h"
#include <iostream>

Route::Route()
{}
Route::~Route()
{}

bool Route::AddRegexpMatcher(std::string pattern, Matcher::RegexpType regexp_type, std::string key)
{
    // 创建匹配器
    Matcher m(pattern, regexp_type, key);
    // 有效才保存
    if (m.IsValid())
    {
        matchers_.push_back(std::move(m));
        return true;
    }
    return false;
}

Route::RoutePtr Route::SetHandler(Handler handler)
{
    handler_ = handler;
    return shared_from_this();
}

Route::RoutePtr Route::SetPath(std::string pattern)
{
    if (!AddRegexpMatcher(pattern, Matcher::REGEXP_TYPE_PATH))
    {
        std::cout << "add regexp matcher failed, please check it again" << std::endl;
        abort();
    }
    return shared_from_this();
}

Route::RoutePtr Route::SetPrefix(std::string pattern)
{
    // 加上{file_path}以便提取参数
    if (!AddRegexpMatcher(pattern + "{file_path}", Matcher::REGEXP_TYPE_PREFIX))
    {
        std::cout << "add regexp matcher failed, please check it again" << std::endl;
        abort();
    }
    return shared_from_this();
}

Route::RoutePtr Route::SetMethod(std::string pattern)
{
    if (!AddRegexpMatcher(pattern, Matcher::REGEXP_TYPE_METHOD))
    {
        std::cout << "add regexp matcher failed, please check it again" << std::endl;
        abort();
    }
    return shared_from_this();
}

Route::RoutePtr Route::SetHeader(std::string key, std::string value)
{
    if (!AddRegexpMatcher(value, Matcher::REGEXP_TYPE_HEADER, key))
    {
        std::cout << "add regexp matcher failed, please check it again" << std::endl;
        abort();
    }
    return shared_from_this();
}

Route::RoutePtr Route::SetQuery(std::string key, std::string value)
{
    if (!AddRegexpMatcher(value, Matcher::REGEXP_TYPE_QUERY, key))
    {
        std::cout << "add regexp matcher failed, please check it again" << std::endl;
        abort();
    }
    return shared_from_this();
}

bool Route::Match(const HttpRequest& request, std::unordered_map<std::string, std::string>* match_map)
{
    // 只有当该路由下的所有匹配条件都匹配成功的话才能返回
    // 否则应该清空match_map
    for (auto matcher : matchers_)
    {
        if (!matcher.Match(request, match_map))
        {
            match_map->clear();
            return false;
        }
    }
    return true;
}