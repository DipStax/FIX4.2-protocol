#pragma once

#include <string>
#include <vector>

namespace utils
{
    bool is_numeric(const std::string &_str);
    bool is_double(const std::string &_str);

    template<class T>
    T to(const std::string &_str);

    template<char C>
    std::vector<std::string> split(const std::string &_str);

    std::string id();

    std::vector<std::string> space_split(const std::string &_str);

    std::string get_timestamp();
}

#include "Shared/Utils/Utils.inl"