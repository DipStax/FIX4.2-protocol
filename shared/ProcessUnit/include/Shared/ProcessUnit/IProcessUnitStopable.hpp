#pragma once

namespace pu
{
    class IProcessUnitStopable
    {
        protected:
            virtual void setup() = 0;
            virtual void onStop() = 0;
    };
}