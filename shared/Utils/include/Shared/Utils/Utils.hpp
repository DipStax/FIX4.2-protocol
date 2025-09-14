#pragma once

#include <string>
#include <vector>
#include <random>
#include <sstream>
#include <array>
#include <iomanip>

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



    struct Uuid
    {
        static std::string Generate() {
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
    };
}

#include "Shared/Utils/Utils.inl"