#ifndef ROUTER_H
#define ROUTER_H

#include "route.h"

class Router
{
public:
    Router();
    ~Router();
    
    // 返回新的子路由
    Route::RoutePtr NewRoute();
    // 遍历所有的路由是否有匹配
    Route::Handler Match(const HttpRequest& request, std::unordered_map<std::string, std::string>* match_map);
    
private:
    // 所有的子路由
    std::vector<Route::RoutePtr> routes_;
};

#endif // ROUTER_H