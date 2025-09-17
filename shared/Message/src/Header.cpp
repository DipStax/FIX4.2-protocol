#include <iomanip>
#include <ctime>
#include <iostream>

#include <sys/time.h>

#include "Shared/Utils/Utils.hpp"
#include "Shared/Message/Header.hpp"
#include "Shared/Message/Tag.hpp"

namespace fix
{
    std::pair<bool, Reject> old_Header::Verify(Serializer::AnonMessage &_msg, const std::string &_sender, const std::string &_target, size_t _seqnum)
    {
        // need to verify sending time
        std::pair<bool, Reject> reject = Has<Tag::BeginString, Tag::BodyLength, Tag::MsqSeqNum, Tag::MsgType, Tag::SenderCompId, Tag::SendingTime, Tag::TargetCompId>(_msg);
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
        reject = verify<Tag::BodyLength>(_msg.at(Tag::BodyLength), old_Message::getBodyLength(body_len)); // calculate body length
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
        reject = verify<Tag::CheckSum>(_msg.at(Tag::CheckSum), old_Message::getChecksum(checksum));
        if (reject.first)
            return reject;
        return reject;
    }

    void old_Header::set9_bodyLength(const std::string &_val)
    {
        BodyLength = _val;
    }

    void old_Header::set9_bodyLength(const size_t &_val)
    {
        BodyLength = std::to_string(_val);
    }

    void old_Header::set35_MsgType(const std::string &_val)
    {
        MsgType = _val;
    }

    void old_Header::set34_msgSeqNum(const std::string &_val)
    {
        MsgSeqNum = _val;
    }

    void old_Header::set34_msgSeqNum(const size_t &_val)
    {
        MsgSeqNum = std::to_string(_val);
    }

    void old_Header::set49_SenderCompId(const std::string &_val)
    {
        SenderCompId = _val;
    }

    void old_Header::set56_TargetCompId(const std::string &_val)
    {
        TargetCompId = _val;
    }

    old_Header::operator std::string() const
    {
        return "8=" + BeginString + (char)FIX_DELIMITER +
               "9=" + BodyLength + (char)FIX_DELIMITER +
               "35=" + MsgType + (char)FIX_DELIMITER +
               "49=" + SenderCompId + (char)FIX_DELIMITER +
               "56=" + TargetCompId + (char)FIX_DELIMITER +
               "34=" + MsgSeqNum + (char)FIX_DELIMITER +
               "52=" + SendingTime + (char)FIX_DELIMITER;
    }

    void old_Header::setSendingTime()
    {
        std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm tm = *std::gmtime(&time);
        int year = 1900 + tm.tm_year;
        int month = tm.tm_mon + 1;
        int day = tm.tm_mday;
        int hour = tm.tm_hour;
        int minute = tm.tm_min;
        int second = tm.tm_sec;
        char *ptr = nullptr;

        SendingTime.clear();
        SendingTime.resize(17);
        ptr = SendingTime.data();
        ptr[0] = '0' + (year / 1000) % 10;
        ptr[1] = '0' + (year / 100) % 10;
        ptr[2] = '0' + (year / 10) % 10;
        ptr[3] = '0' + (year % 10);
        ptr[4] = '0' + month / 10;
        ptr[5] = '0' + month % 10;
        ptr[6] = '0' + day / 10;
        ptr[7] = '0' + day % 10;
        ptr[8] = '-';
        ptr[9]  = '0' + hour / 10;
        ptr[10] = '0' + hour % 10;
        ptr[11] = ':';
        ptr[12] = '0' + minute / 10;
        ptr[13] = '0' + minute % 10;
        ptr[14] = ':';
        ptr[15] = '0' + second / 10;
        ptr[16] = '0' + second % 10;
    }

    uint64_t old_Header::getTargetCompId() const
    {
        return utils::to<uint64_t>(TargetCompId);
    }

    void old_Header::updateMsgSeqNum()
    {
        MsgSeqNum = std::to_string(std::stoi(MsgSeqNum) + 1);
    }

    std::string old_Header::getPartialHeader() const
    {
        return "35=" + MsgType + (char)FIX_DELIMITER +
               "49=" + SenderCompId + (char)FIX_DELIMITER +
               "56=" + TargetCompId + (char)FIX_DELIMITER +
               "34=" + MsgSeqNum + (char)FIX_DELIMITER +
               "52=" + SendingTime + (char)FIX_DELIMITER;
    }
}