#include <iomanip>
#include <ctime>
#include <iostream>

#include <sys/time.h>

#include "Common/Core/Utils.hpp"
#include "Common/Message/Header.hpp"
#include "Common/Message/Tag.hpp"

namespace fix
{
    std::pair<bool, Reject> Header::Verify(Serializer::AnonMessage &_msg, const UserId &_sender, const UserId &_target, size_t _seqnum)
    {
        // need to verify sending time
        std::pair<bool, Reject> reject = utils::Has<Tag::BeginString, Tag::BodyLength, Tag::MsqSeqNum, Tag::MsgType, Tag::SenderCompId, Tag::SendingTime, Tag::TargetCompId>(_msg);
        std::string body_len{};
        std::string checksum{};

        for (const auto &[_key, _val] : _msg)
        {
            if (_key == Tag::CheckSum)
                continue;
            if (_key != Tag::BodyLength && _key != Tag::BeginString)
                body_len += (_key + "=" + _val + (char)FIX_DELIMITER);
            checksum += (_key + "=" + _val + (char)FIX_DELIMITER);
        }
        if (reject.first)
            return reject;
        reject = verify_all<Tag::BeginString, Tag::MsgType, Tag::SendingTime>(_msg);
        if (reject.first)
            return reject;
        reject = verify<Tag::BodyLength>(_msg.at(Tag::BodyLength), Message::getBodyLength(body_len)); // calculate body length
        if (reject.first)
            return reject;
        if (!_sender.empty()) {
            reject = verify<Tag::SenderCompId>(_msg.at(Tag::SenderCompId), _sender);
            if (reject.first)
                return reject;
            reject = verify<Tag::MsqSeqNum>(_msg.at(Tag::MsqSeqNum), _seqnum);
            if (reject.first)
                return reject;
        }
        reject = verify<Tag::TargetCompId>(_msg.at(Tag::TargetCompId), _target);
        if (reject.first)
            return reject;
        reject = verify<Tag::CheckSum>(_msg.at(Tag::CheckSum), Message::getChecksum(checksum));
        if (reject.first)
            return reject;
        return reject;
    }

    void Header::set9_bodyLength(const std::string &_val)
    {
        BodyLength = _val;
    }

    void Header::set9_bodyLength(const size_t &_val)
    {
        BodyLength = std::to_string(_val);
    }

    void Header::set35_MsgType(const std::string &_val)
    {
        MsgType = _val;
    }

    void Header::set34_msgSeqNum(const std::string &_val)
    {
        MsgSeqNum = _val;
    }

    void Header::set34_msgSeqNum(const size_t &_val)
    {
        MsgSeqNum = std::to_string(_val);
    }

    void Header::set49_SenderCompId(const std::string &_val)
    {
        SenderCompId = _val;
    }

    void Header::set56_TargetCompId(const std::string &_val)
    {
        TargetCompId = _val;
    }

    Header::operator std::string() const
    {
        return "8=" + BeginString + (char)FIX_DELIMITER +
               "9=" + BodyLength + (char)FIX_DELIMITER +
               "35=" + MsgType + (char)FIX_DELIMITER +
               "49=" + SenderCompId + (char)FIX_DELIMITER +
               "56=" + TargetCompId + (char)FIX_DELIMITER +
               "34=" + MsgSeqNum + (char)FIX_DELIMITER +
               "52=" + SendingTime + (char)FIX_DELIMITER;
    }

    void Header::setSendingTime()
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
        // std::string formattedTime(timeBuffer);
        SendingTime = formattedTime;
    }

    uint64_t Header::getTargetCompId() const
    {
        return utils::to<uint64_t>(TargetCompId);
    }

    void Header::updateMsgSeqNum()
    {
        MsgSeqNum = std::to_string(std::stoi(MsgSeqNum) + 1);
    }

    std::string Header::getPartialHeader() const
    {
        return "35=" + MsgType + (char)FIX_DELIMITER +
               "49=" + SenderCompId + (char)FIX_DELIMITER +
               "56=" + TargetCompId + (char)FIX_DELIMITER +
               "34=" + MsgSeqNum + (char)FIX_DELIMITER +
               "52=" + SendingTime + (char)FIX_DELIMITER;
    }
}