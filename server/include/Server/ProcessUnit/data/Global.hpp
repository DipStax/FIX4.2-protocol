#pragma once

#include "Server/ProcessUnit/data/Context.hpp"

#include "Shared/Thread/Queue.hpp"
#include "Shared/Message-v2/Message.hpp"

namespace data
{
    struct UnparsedMessage
    {
        UnparsedMessage() = default;
        UnparsedMessage(UnparsedMessage &&_data) noexcept = default;
        UnparsedMessage(const UnparsedMessage &_data) = default;
        UnparsedMessage(const fix42::Header &&_header, const fix::MapMessage &&_message) noexcept;
        virtual ~UnparsedMessage() = default;

        UnparsedMessage &operator=(UnparsedMessage &&_data) noexcept = default;

        fix42::Header Header;       ///< Message header
        fix::MapMessage Message{};  ///< Message not parsed.
    };

    struct StringOutput
    {
        StringOutput() = default;
        StringOutput(StringOutput &&_data) noexcept = default;
        StringOutput(const StringOutput &_data) = default;
        StringOutput(char _msgtype, const std::string &&_msg) noexcept;
        virtual ~StringOutput() = default;

        StringOutput &operator=(StringOutput &&_data) noexcept = default;

        char MessageType;
        std::string Message{};  ///< FIX formated message into string
    };
}

using UnparsedMessageQueue = ts::Queue<Context<data::UnparsedMessage>>;
using StringOutputQueue = ts::Queue<Context<data::StringOutput>>;