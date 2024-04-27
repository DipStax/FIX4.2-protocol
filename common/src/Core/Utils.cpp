#include <numeric>
#include <sstream>

#include "Common/Core/Utils.hpp"
#include <random>
#include <iomanip>

namespace utils
{
    bool is_numeric(const std::string &_str)
    {
        std::string::const_iterator it = _str.begin();

        for (; it != _str.end() && std::isdigit(*it); it++) {}
        return !_str.empty() && it == _str.end();
    }

    bool is_double(const std::string &_str)
    {
        std::istringstream stream(_str);
        double f = 0.f;

        stream >> std::noskipws >> f;
        return stream.eof() && !stream.fail();
    }

    std::string id() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 9999999);

        unsigned int id = dis(gen);
        std::string idStr = std::to_string(id);

        while (idStr.length() < 7) {
            idStr = "0" + idStr;
        }

        return idStr;
    }

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

    std::string get_timestamp()
    {
        time_t currentTime = time(nullptr);
        struct tm *timeInfo = localtime(&currentTime);

        // Get the microseconds since the last second
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        int microseconds = tv.tv_usec;

        // Format the current time as a string
        char dateTimeBuffer[80];
        strftime(dateTimeBuffer, sizeof(dateTimeBuffer), "%Y%m%d-%H:%M:%S", timeInfo);

        // Convert the formatted time string to a C++ string
        std::string formattedTime = dateTimeBuffer;

        // Prepend two zeros to the microsecond value
        std::string microsecondsString = std::to_string(microseconds);
        if (microsecondsString.length() < 3)
            microsecondsString = "00" + microsecondsString;

        // Combine the formatted time string with the microsecond value
        formattedTime = formattedTime + "." + microsecondsString.substr(0, 3);
        return formattedTime;
    }
}