#pragma once

#include "Server/Core/ProcessUnit/data/Context.hpp"
#include "Server/Core/ProcessUnit/data/Global.hpp"

namespace data
{
    // struct OBActionInput
    // {
    //     enum class Action
    //     {
    //         Add,
    //         Modify,
    //         Cancel
    //     };

    //     OBActionInput() = default;
    //     OBActionInput(OBActionInput &&_data) noexcept = default;
    //     OBActionInput(const OBActionInput &_data) = default;
    //     virtual ~OBActionInput() = default;

    //     OBActionInput &operator=(OBActionInput &&_data) noexcept = default;

    //     Action action;
    //     OrderType type;
    //     Price price;
    //     Price oprice;
    //     OrderId target;
    //     Order order;
    // };

    using MarketRouterInput = RouterInput;
    using OBActionInput = RouterInput;

    struct RefreshSubInput
    {
        RefreshSubInput() = default;
        RefreshSubInput(RefreshSubInput &&_data) noexcept = default;
        RefreshSubInput(const RefreshSubInput &_data) = default;
        virtual ~RefreshSubInput() = default;

        RefreshSubInput &operator=(RefreshSubInput &&_data) noexcept = default;

        std::string Id;
        uint8_t SubType;
        uint8_t Depth;
        uint8_t UpdateType;
        std::vector<OrderType> Types;
    };
}

using InputOBAction = ts::Queue<Context<data::OBActionInput>>;