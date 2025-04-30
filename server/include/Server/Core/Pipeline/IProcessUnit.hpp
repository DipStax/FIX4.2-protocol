#pragma once

#include <stop_token>

class IProcessUnitBase
{
    protected:
        virtual void runtime(std::stop_token _st) = 0;
};

template<class T>
class IProcessUnit : IProcessUnitBase
{
    public:
        using InputType = ts::Queue<T>;

        InputType &getInput();
};
