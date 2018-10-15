#ifndef ROUTER_H
#define ROUTER_H

#include "route.h"
#include <memory>


class Router
{
public:
    using RoutePtr = std::shared_ptr<Route>;
    Router();
    ~Router();
    
    RoutePtr NewRoute()
    {
        auto r = std::make_shared<Route>();
        routes_.push_back(r);
        return r;
    }

    Route::Handler Match(const HttpRequest& request, std::map<std::string, std::string>* match_map)
    {
        for (auto &r : routes_)
        {
            if (r->Match(request, match_map))
            {
                return r->GetHandler();
            }
        }
        return nullptr;
    }

    
private:
    std::vector<RoutePtr> routes_;
};

#endif // ROUTER_H