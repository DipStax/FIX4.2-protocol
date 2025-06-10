#pragma once

#include "Common/Network/Buffer.hpp"
#include "Common/Container/ProcessUnit.hpp"

namespace ipc
{
    struct Helper
    {
        static net::Buffer Status(PUStatus _status);
    };
}