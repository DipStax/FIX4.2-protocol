#include <sstream>

#include "Common/Core/Utils.hpp"
#include "Common/Message/Reject.hpp"

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

    template<const char *T, const char *...Ts>
    std::pair<bool, fix::Reject> Has(fix::Serializer::AnonMessage &_msg)
    {
        std::pair<bool, fix::Reject> reject = { false, {} };

        if (!_msg.contains(T)) {
            reject.first = true;
            reject.second.set371_refTagId(T);
            reject.second.set373_sessionRejectReason(fix::Reject::ReqTagMissing);
            reject.second.set58_text("Unable to find required field");
        } else if (_msg.at(T).empty()) {
            reject.first = true;
            reject.second.set371_refTagId(T);
            reject.second.set373_sessionRejectReason(fix::Reject::EmptyValue);
            reject.second.set58_text("Waiting a value");
        } else {
            if constexpr (sizeof...(Ts) == 0)
                return { false, fix::Reject{} };
            else
                return utils::Has<Ts...>(_msg);
        }
        return reject;
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