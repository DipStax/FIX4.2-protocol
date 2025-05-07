#pragma once

#include "Server/Core/ProcessUnit/data/Context.hpp"

#include "Common/Message/Fix.hpp"
#include "Common/Message/Serializer.hpp"
#include "Common/Network/UDPPackage.hpp"
#include "Common/Thread/Queue.hpp"

namespace data
{
    /// @brief Data transfered from the pip::InNetwork pipeline to the pip::Action pipeline.
    struct RouterInput
    {
        RouterInput() = default;
        RouterInput(RouterInput &&_data) noexcept = default;
        RouterInput(const RouterInput &_data) = default;
        RouterInput(const fix::Serializer::AnonMessage &&_message);
        virtual ~RouterInput() = default;

        RouterInput &operator=(RouterInput &&_data) noexcept = default;

        fix::Serializer::AnonMessage Message{};     ///< Undefined message data.
    };

    /// @brief Data transfered from the pip::Market pipeline to the pip::OutNetwork pipeline
    struct OutNetworkInput
    {
        OutNetworkInput() = default;
        OutNetworkInput(OutNetworkInput &&_data) noexcept = default;
        OutNetworkInput(const OutNetworkInput &_data) = default;
        OutNetworkInput(const fix::Message &&_msg) noexcept;
        virtual ~OutNetworkInput() = default;

        OutNetworkInput &operator=(OutNetworkInput &&_data) noexcept = default;

        fix::Message Message{};                     ///< Final message send to the client.
    };
}

using InputRouter = ts::Queue<Context<data::RouterInput>>;
using InputNetworkOutput = ts::Queue<Context<data::OutNetworkInput>>;
using InputUdp = ts::Queue<data::UDPPackage>;