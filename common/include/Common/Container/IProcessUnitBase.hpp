#pragma once

#include <stop_token>

namespace pu
{
    class IProcessUnitBase
    {
        protected:
            virtual void runtime(std::stop_token _st) = 0;
    };
}