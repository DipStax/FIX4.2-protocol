#pragma once

#include "Common/Log/ILogger.hpp"

namespace log::imp
{
    class Base : public ILogger
    {
        public:
            Base(const std::string &_name);
            virtual ~Base() = default;

            const std::string Name;
    };
}