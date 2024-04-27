#include <numeric>

#include "Common/Message/Fix.hpp"
#include "Common/Core/Utils.hpp"
#include "Common/Message/Tag.hpp"
#include <iostream>

namespace fix
{
    Message::operator std::string()
    {
        return to_string();
    }

    std::string Message::to_string()
    {
        std::string body_tmp = "";
        std::string header_tmp = "";
        std::string msg = "";

        for (const auto &[_key, _val] : m_params)
            body_tmp += _key + "=" + _val + (char)FIX_DELIMITER;

        header.setSendingTime();
        // header.updateMsgSeqNum();

        std::size_t _len = Message::getBodyLength(
            header.getPartialHeader() + body_tmp
        );

        header.set9_bodyLength(std::to_string(_len));
        header_tmp = header;

        msg = header_tmp + body_tmp;
        msg += "10=" + Message::getChecksum(msg) + (char)FIX_DELIMITER;

        return msg;
    }

    std::size_t Message::getBodyLength(const std::string &_str)
    {
        return _str.size();
    }

    std::string Message::getChecksum(const std::string &_str)
    {
        size_t sum = 0;
        std::string formated_checksum{};

        sum = std::accumulate(_str.begin(), _str.end(), 0) % 256;
        if (sum < 10)
            formated_checksum = "00";
        else if (sum < 100)
            formated_checksum = "0";
        formated_checksum += std::to_string(sum);
        return formated_checksum;
    }
}