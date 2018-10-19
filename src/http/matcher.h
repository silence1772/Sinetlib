#ifndef MATCHER_H
#define MATCHER_H

#include <vector>
#include <string>
#include <unordered_map>

class HttpRequest;

class Matcher
{
public:
    // 匹配类型
    enum RegexpType
    {
        // 根据请求方法匹配
        REGEXP_TYPE_METHOD,
        // 根据路径匹配
        REGEXP_TYPE_PATH,
        // 根据路径前缀匹配
        REGEXP_TYPE_PREFIX,
        // 根据头部信息匹配
        REGEXP_TYPE_HEADER,
        // 根据查询参数匹配
        REGEXP_TYPE_QUERY
    };

    Matcher(std::string pattern, RegexpType regexp_type, std::string key);

    // 是否有效
    bool IsValid() const { return is_valid_; }
    // 是否匹配
    bool Match(const HttpRequest& request, std::unordered_map<std::string, std::string>* match_map);

private:
    // 初始化
    void Init(std::string pattern);

    // 该匹配条件是否有效
    bool is_valid_;
    // 匹配类型
    RegexpType regexp_type_;
    // 根据头部信息或查询参数匹配时使用，表示key:value键值对中的key
    std::string key_;
    // 要匹配的正则表达式
    std::string regexp_pattern_;
    // 正则表达式中匹配的各个条件对应的名称
    std::vector<std::string> regexp_names_;
};

#endif // MATCHER_H