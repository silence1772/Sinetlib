#include "matcher.h"
#include <regex>
#include <iostream>
Matcher::Matcher(std::string pattern, RegexpType regexp_type, std::string key) :
    is_valid_(true),
    regexp_type_(regexp_type),
    key_(key)
{
    Init(pattern);
}

void Matcher::Init(std::string pattern)
{
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
        if (j == end)
        {
            regexp_names_.push_back(pattern.substr(indexs[i]+1, (end-1)-(indexs[i]+1) ) );
            regexp_pattern_.append("(.*)");
        }
    }

    if (end < pattern.size())
    {
        regexp_pattern_.append(pattern.substr(end, pattern.size() - end));
    }

    // std::cout << regexp_pattern_ << std::endl;
    // for (auto i : regexp_names_)
    // {
    //     std::cout << i << std::endl;
    // }
}

bool Matcher::Match(const HttpRequest& request)
{
    std::regex reg(regexp_pattern_);
    std::smatch m;

    if (regexp_type_ == REGEXP_TYPE_PATH || regexp_type_ == REGEXP_TYPE_PREFIX)
    {
        if (regex_match(request.GetPath(), m, reg))
        {
            for (int i = 0; i < m.size(); ++i)
                std::cout << m[i] << std::endl;
            return true;
        }
    }
    return false;
}