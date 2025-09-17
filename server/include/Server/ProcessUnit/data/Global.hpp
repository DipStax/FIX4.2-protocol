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
        fix::MapMessage Message{};  ///< Message not parsed
    };

    struct StringOutput
    {
        StringOutput() = default;
        StringOutput(StringOutput &&_data) noexcept = default;
        StringOutput(const StringOutput &_data) = default;
        StringOutput(char _msgtype, const std::string &&_msg) noexcept;
        virtual ~StringOutput() = default;

        StringOutput &operator=(StringOutput &&_data) noexcept = default;

        char MessageType;       ///< Message type of the FIX formated message
        std::string Message{};  ///< FIX formated message into string
    };

    template<class T>
    struct ParsedMessage
    {
        ParsedMessage() = default;
        ParsedMessage(ParsedMessage &&_data) noexcept = default;
        ParsedMessage(const ParsedMessage &_data) = default;
        ParsedMessage(const fix42::Header &&_header, const T &&_message) noexcept;
        virtual ~ParsedMessage() = default;

        ParsedMessage &operator=(ParsedMessage &&_data) noexcept = default;

        fix42::Header Header;   ///< Message header
        T Message{};            ///< Parsed message
    };
}

using UnparsedMessageQueue = ts::Queue<Context<data::UnparsedMessage>>;
using StringOutputQueue = ts::Queue<Context<data::StringOutput>>;
template<class T>
using MessageQueue = ts::Queue<Context<data::ParsedMessage<T>>>;

#include "Server/ProcessUnit/data/Global.inl"