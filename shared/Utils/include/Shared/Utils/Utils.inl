#include <sstream>

#include "Shared/Utils/Utils.hpp"

namespace utils
{
    template<class T>
    T to(const std::string &_str)
    {
        T val{};
        std::istringstream stream(_str);

        stream >> std::ref(val);
        return val;
    }

    template<char C>
    std::vector<std::string> split(const std::string &_str)
    {
        std::vector<std::string> result;
        std::istringstream ss(_str);
        std::string token;

        while (std::getline(ss, token, C))
            result.push_back(token);
        return result;
    }
}