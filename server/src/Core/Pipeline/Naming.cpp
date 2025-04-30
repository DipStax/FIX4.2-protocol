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

RouterInput &RouterInput::operator=(RouterInput &&_data) noexcept
{
    if (this != &_data)
        Message = std::move(_data.Message);
    return *this;
}

MarketInput::MarketInput(MarketInput &&_data) noexcept
    : OrderData(std::move(_data.OrderData))
{
}

MarketInput::MarketInput(const MarketInput &_data)
    : OrderData(_data.OrderData)
{
}

MarketInput &MarketInput::operator=(MarketInput &&_data) noexcept
{
    if (this != &_data)
        OrderData = std::move(_data.OrderData);
    return *this;
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

MarketDataInput::MarketDataInput(const MarketDataInput &&_data) noexcept
    : MarketDataInputData(std::move(_data))
{
}

MarketDataInput &MarketDataInput::operator=(MarketDataInput &&_data) noexcept
{
    if (this != &_data)
        MarketDataInputData::operator=(std::move(_data));
    return *this;
}