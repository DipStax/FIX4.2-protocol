#include "Server/Core/Pipeline/Naming.hpp"

RouterInput::RouterInput(const fix::Serializer::AnonMessage &&_msg)
    : Message(std::move(_msg))
{
}

OutNetworkInput::OutNetworkInput(const fix::Message &&_msg) noexcept
    : Message(std::move(_msg))
{
}