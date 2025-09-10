#pragma once

#include "Shared/ProcessUnit/AProcessUnitBase.hpp"

#include "Shared/Thread/Queue.hpp"

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

#include "Shared/ProcessUnit/AProcessUnit.inl"