#pragma once

#include <stop_token>

class IProcessUnit
{
    protected:
        virtual void runtime(std::stop_token _st) = 0;
};
