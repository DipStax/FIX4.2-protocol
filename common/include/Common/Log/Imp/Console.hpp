#pragma once

#include <format>

#include "Common/Log/Imp/Base.hpp"

namespace log::imp
{
    class Console : public Base
    {
        public:
            Console(const std::string &_name);
            ~Console() = default;

            void newEventLog(Event _event);
    };
}