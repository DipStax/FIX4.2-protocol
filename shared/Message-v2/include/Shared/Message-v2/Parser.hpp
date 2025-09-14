#pragma once

#include "Shared/Message-v2/Message.hpp"

#include "FIX-Message/Expected.hpp"

namespace fix42
{
    template<class T>
    xstd::Expected<T, fix42::msg::SessionReject> parseMessage(const fix::MapMessage &_msg, const fix42::Header &_header);
}

#include "Shared/Message-v2/Parser.inl"