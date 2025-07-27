#pragma once

#include "Shared/Log/ILogger.hpp"

namespace logger::imp
{
    class Base : public ILogger
    {
        public:
            Base(const std::string &_name);
            virtual ~Base() = default;

            const std::string Name;
    };
}