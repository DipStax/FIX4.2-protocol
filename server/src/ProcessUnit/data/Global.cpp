#include "Server/ProcessUnit/data/Global.hpp"

namespace data
{
    RouterInput::RouterInput(const fix::Serializer::AnonMessage &&_msg)
        : Message(std::move(_msg))
    {
    }

    OutNetworkInput::OutNetworkInput(const fix::old_Message &&_msg) noexcept
        : Message(std::move(_msg))
    {
    }
}