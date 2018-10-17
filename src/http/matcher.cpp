#include "matcher.h"
#include "httprequest.h"
#include <regex>
#include <iostream>
#include <utility>

Matcher::Matcher(std::string pattern, RegexpType regexp_type, std::string key) :
    is_valid_(true),
    regexp_type_(regexp_type),
    key_(key)
{
    // 从pattern中提取出正则表达式
    Init(pattern);
}

void Matcher::Init(std::string pattern)
{
    // 判断是否成形良好，即{}是否配对
    int level = 0, index = 0;
    std::vector<int> indexs;
    for (int i = 0; i < pattern.size(); ++i)
    {
        if (pattern[i] == '{')
        {
            if (++level == 1)
                index = i;
        }
        else if (pattern[i] == '}')
        {
            if (--level == 0)
            {
                indexs.push_back(index);
                indexs.push_back(i + 1);
            }
            else if (level < 0)
            {
                is_valid_ = false;
                return;
            }
        }
    }
    if (level != 0)
    {
        is_valid_ = false;
        return;
    }

    // 将类似{name:[a-z]+}中name提取出来，然后将正则条件[a-z]+加()后添加到正则表达式里面
    // aa{name:[a-z]+}cc 经过处理后得到正则表达式 aa([a-z]+)cc
    int end = 0;
    for (int i = 0; i < indexs.size(); i += 2)
    {
        std::string raw = pattern.substr(end, indexs[i] - end);
        end = indexs[i+1];
        regexp_pattern_.append(raw);

        int j;
        for (j = indexs[i]; j < end; ++j)
        {
            if (pattern[j] == ':')
            {
                regexp_names_.push_back(pattern.substr(indexs[i]+1, j-(indexs[i]+1) ) );
                regexp_pattern_.append('(' + pattern.substr(j+1, (end-1)-(j+1) ) + ')' );
                break;
            }
        }
        // 如{name}没有正则条件的直接添加通配条件
        if (j == end)
        {
            regexp_names_.push_back(pattern.substr(indexs[i]+1, (end-1)-(indexs[i]+1) ) );
            regexp_pattern_.append("(.*)");
        }
    }
    // 处理余下的字符串
    if (end < pattern.size())
    {
        regexp_pattern_.append(pattern.substr(end, pattern.size() - end));
    }
}

bool Matcher::Match(const HttpRequest& request, std::map<std::string, std::string>* match_map)
{
    std::regex reg(regexp_pattern_);
    std::smatch m;

    if (regexp_type_ == REGEXP_TYPE_PATH || regexp_type_ == REGEXP_TYPE_PREFIX)
    {
        if (regex_match(request.GetPath(), m, reg))
        {
            // m里面包含匹配的整个字符串以及各个匹配子串
            // 例如 aa{name:.*}cc 经过处理后得到正则表达式 aa(.*)cc 以及对应的名称name
            // aa(.*)cc 匹配aabbcc会得到包含aabbcc、bb两个字符串的m
            // 因此通过倒序将名称和值组合在一起，即name:bb
            for (int i = regexp_names_.size()-1, j = m.size()-1; i >= 0 && j >= 0; --i, --j)
            {
                match_map->insert(std::make_pair(regexp_names_[i], m[j]));
            }
            return true;
        }
    }
    else if (regexp_type_ == REGEXP_TYPE_METHOD)
    {
        if (regex_match(request.GetMethodStr(), reg))
        {
            return true;
        }
    }
    else if (regexp_type_ == REGEXP_TYPE_HEADER)
    {
        if (regex_match(request.GetHeader(key_), reg))
        {
            return true;
        }
    }
    else if (regexp_type_ == REGEXP_TYPE_QUERY)
    {
        if (regex_match(request.GetQuery(key_), reg))
        {
            return true;
        }
    }
    return false;
}