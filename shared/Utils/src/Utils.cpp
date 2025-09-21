#include <numeric>
#include <sstream>
#include <random>
#include <iomanip>

#include <sys/time.h>

#include "Shared/Utils/Utils.hpp"

namespace utils
{
    std::vector<std::string> space_split(const std::string &_str)
    {
        std::stringstream stream(_str);
        std::string value;
        std::vector<std::string> result;

        while (stream) {
            stream >> value;
            if (stream)
                result.emplace_back(value);
        }
        return result;
    }

    std::string trunc(const std::string &_str, size_t _size)
    {
        return _str.substr(0, _size) + "...";
    }

    std::string Uuid::Generate()
    {
        static thread_local std::random_device rd;
        static thread_local std::mt19937_64 gen(rd());
        static thread_local std::uniform_int_distribution<unsigned int> dist(0, 255);

        std::array<unsigned char, 16> bytes{};

        for (auto &b : bytes)
            b = static_cast<unsigned char>(dist(gen));
        std::ostringstream oss;

        bytes[6] = (bytes[6] & 0x0F) | 0x40;
        bytes[8] = (bytes[8] & 0x3F) | 0x80;
        oss << std::hex << std::setfill('0');
        for (size_t i = 0; i < bytes.size(); i++) {
            oss << std::setw(2) << static_cast<int>(bytes[i]);
            if (i == 3 || i == 5 || i == 7 || i == 9) {
                oss << "-";
            }
        }

        return oss.str();
    }
}