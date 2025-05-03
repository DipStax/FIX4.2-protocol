#pragma once

#include "Server/Core/ProcessUnit/interface/IProcessUnitBase.hpp"

#include "Common/Thread/Queue.hpp"

namespace pu
{
    template<class T>
    class IProcessUnit : IProcessUnitBase
    {
        public:
            using InputType = T;
            using QueueInputType = ts::Queue<T>;

            virtual QueueInputType &getInput() = 0;
    };


}
