#pragma once

namespace pu
{
    class IProcessUnitStopable
    {
        protected:
            virtual void onStop() = 0;
    };
}