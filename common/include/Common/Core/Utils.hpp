#pragma once

#include <string>

#include "Common/Message/Serializer.hpp"
#include "Common/Message/Reject.hpp"

namespace utils
{
    bool is_numeric(const std::string &_str);
    bool is_double(const std::string &_str);

    template<class T>
    T to(const std::string &_str);
    template<const char *T, const char *...Ts>
    std::pair<bool, fix::Reject> Has(fix::Serializer::AnonMessage &_msg);
    template<char C>
    std::vector<std::string> split(const std::string &_str);

    std::string id();

    std::vector<std::string> space_split(const std::string &_str);

    std::string get_timestamp();
}

#include "Common/Core/Utils.inl"