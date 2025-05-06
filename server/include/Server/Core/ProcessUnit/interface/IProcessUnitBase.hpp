#pragma once

#include <stop_token>

namespace pu
{
    class IProcessUnitBase
    {
        protected:
            virtual std::string getThreadName() const = 0;

            virtual void runtime(std::stop_token _st) = 0;
    };
}