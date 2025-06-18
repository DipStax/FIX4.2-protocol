#pragma once

#include "Common/Container/AProcessUnitBase.hpp"

#include "Common/Thread/Queue.hpp"

namespace pu
{
    template<class T>
    class AProcessUnit : public AProcessUnitBase
    {
        public:
            AProcessUnit(const std::string &_name);
            virtual ~AProcessUnit() = default;

            using InputType = T;
            using QueueInputType = ts::Queue<T>;

            virtual QueueInputType &getInput() = 0;
    };
}

#include "Common/Container/AProcessUnit.inl"