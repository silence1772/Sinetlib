#ifndef ROUTE_H
#define ROUTE_H

#include <vector>
#include <functional>
#include <memory>
#include "matcher.h"
#include "httpresponse.h"

class Route : public std::enable_shared_from_this<Route>
{
public:
    using RoutePtr = std::shared_ptr<Route>;
    using Handler = std::function<void(const HttpRequest&, HttpResponse*)>;
    //using Handler = std::function<void(const HttpRequest&)>;
    Route();
    ~Route();

    bool AddRegexpMatcher(std::string pattern, Matcher::RegexpType regexp_type, std::string key = "");

    RoutePtr SetHandler(Handler handler);
    RoutePtr SetPath(std::string pattern);
    RoutePtr SetPrefix(std::string pattern);
    RoutePtr SetMethod(std::string pattern);
    RoutePtr SetHeader(std::string key, std::string value);
    RoutePtr SetQuery(std::string key, std::string value);

    Handler GetHandler() const { return handler_; }

    bool Match(const HttpRequest& request);

private:
    Handler handler_;
    std::vector<Matcher> matchers_;
};

#endif // ROUTE_H