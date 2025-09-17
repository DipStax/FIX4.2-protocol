#include "Server/ProcessUnit/data/Global.hpp"

namespace data
{
    UnparsedMessage::UnparsedMessage(const fix42::Header &&_header, const fix::MapMessage &&_msg) noexcept
        : Header(std::move(_header)), Message(std::move(_msg))
    {
    }

    StringOutput::StringOutput(char _msgtype, const std::string &&_msg) noexcept
        : MessageType(_msgtype), Message(std::move(_msg))
    {
    }
}