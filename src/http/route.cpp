#include "route.h"
#include <iostream>

Route::Route()
{}

Route::~Route()
{}

bool Route::AddRegexpMatcher(std::string pattern, Matcher::RegexpType regexp_type, std::string key)
{
    Matcher m(pattern, regexp_type, key);
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
    if (!AddRegexpMatcher(pattern + "{.*}", Matcher::REGEXP_TYPE_PREFIX))
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

bool Route::Match(const HttpRequest& request)
{
    for (auto matcher : matchers_)
    {
        if (!matcher.Match(request))
            return false;
    }
    return true;
}