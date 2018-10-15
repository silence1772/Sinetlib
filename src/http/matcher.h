#ifndef REGEXPER_H
#define REGEXPER_H

#include <vector>
#include <string>
#include "httprequest.h"

class Matcher
{
public:
    enum RegexpType
    {
        REGEXP_TYPE_METHOD,
        REGEXP_TYPE_PATH,
        REGEXP_TYPE_PREFIX,
        REGEXP_TYPE_HEADER,
        REGEXP_TYPE_QUERY
    };

    Matcher(std::string pattern, RegexpType regexp_type, std::string key);

    bool IsValid() const { return is_valid_; }

    bool Match(const HttpRequest& request, std::map<std::string, std::string>* match_map);

private:
    void Init(std::string pattern);

    bool is_valid_;

    RegexpType regexp_type_;
    std::string key_;
    std::string regexp_pattern_;
    std::vector<std::string> regexp_names_;
};

#endif // REGEXPER_H