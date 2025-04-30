#include "Server/Core/Pipeline/Naming.hpp"

RouterInput::RouterInput(RouterInput &&_data) noexcept
    : Message(std::move(_data.Message))
{
}

RouterInput::RouterInput(const RouterInput &_data)
    : Message(_data.Message)
{
}

RouterInput::RouterInput(const fix::Serializer::AnonMessage &&_msg)
    : Message(std::move(_msg))
{
}

MarketInput::MarketInput(MarketInput &&_data) noexcept
    : type(_data.type)
{
    if (type == Type::Order) {
        OrderData = std::move(_data.OrderData);
    } else {
        RefreshData = std::move(_data.RefreshData);
    }
}

MarketInput &MarketInput::operator=(MarketInput &&_data) noexcept
{
    if (this != &_data) {
        type = _data.type;
        if (type == Type::Order) {
            OrderData = std::move(_data.OrderData);
        } else {
            RefreshData = std::move(_data.RefreshData);
        }
    }
    return *this;
}

MarketInput::MarketInput(const MarketInput &_data)
    : type(_data.type)
{
    if (type == Type::Order) {
        OrderData = _data.OrderData;
    } else {
        RefreshData = _data.RefreshData;
    }
}


OutNetworkInput::OutNetworkInput(OutNetworkInput &&_data) noexcept
    : Message(std::move(_data.Message))
{
}

OutNetworkInput::OutNetworkInput(const OutNetworkInput &_data)
    : Message(_data.Message)
{
}

OutNetworkInput::OutNetworkInput(const fix::Message &&_msg) noexcept
    : Message(std::move(_msg))
{
}

OutNetworkInput &OutNetworkInput::operator=(OutNetworkInput &&_data) noexcept
{
    if (this != &_data)
        Message = std::move(_data.Message);
    return *this;
}