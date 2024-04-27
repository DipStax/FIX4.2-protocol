#include "Server/Core/Pipeline/Naming.hpp"

ActionInput::ActionInput(const ActionInput &&_data) noexcept
    : Client(std::move(_data.Client)), Message(std::move(_data.Message))
{
}

ActionInput::ActionInput(const ClientSocket &_client, const fix::Serializer::AnonMessage &&_msg)
    : Client(_client), Message(std::move(_msg))
{
}

ActionInput &ActionInput::operator=(ActionInput &&_data) noexcept
{
    if (this != &_data) {
        Client = std::move(_data.Client);
        Message = std::move(_data.Message);
    }
    return *this;
}

MarketInput::MarketInput(const MarketInput &&_data) noexcept
    : Client(std::move(_data.Client)), OrderData(std::move(_data.OrderData))
{
}

MarketInput::MarketInput(const MarketInput &_data)
    : Client(_data.Client), OrderData(_data.OrderData)
{
}

MarketInput::MarketInput(const ClientSocket &&_client) noexcept
    : Client(std::move(_client))
{
}

MarketInput &MarketInput::operator=(MarketInput &&_data) noexcept
{
    if (this != &_data) {
        Client = std::move(_data.Client);
        OrderData = std::move(_data.OrderData);
    }
    return *this;
}

OutNetworkInput::OutNetworkInput(const OutNetworkInput &&_data) noexcept
    : Client(std::move(_data.Client)), Message(std::move(_data.Message))
{
}

OutNetworkInput::OutNetworkInput(const OutNetworkInput &_data)
    : Client(_data.Client), Message(_data.Message)
{
}

OutNetworkInput::OutNetworkInput(const ClientSocket &&_client, const fix::Message &&_msg) noexcept
    : Client(std::move(_client)), Message(std::move(_msg))
{
}

OutNetworkInput::OutNetworkInput(const ClientSocket &_client, const fix::Message &_msg)
    : Client(_client), Message(_msg)
{
}

OutNetworkInput &OutNetworkInput::operator=(OutNetworkInput &&_data) noexcept
{
    if (this != &_data) {
        Client = std::move(_data.Client);
        Message = std::move(_data.Message);
    }
    return *this;
}

MarketDataInput::MarketDataInput(const MarketDataInput &&_data) noexcept
    : MarketDataInputData(std::move(_data)), Client(std::move(_data.Client))
{
}

MarketDataInput::MarketDataInput(const MarketDataInputData &&_data, const ClientSocket &&_client) noexcept
    : MarketDataInputData(std::move(_data)), Client(std::move(_client))
{
}

MarketDataInput &MarketDataInput::operator=(MarketDataInput &&_data) noexcept
{
    if (this != &_data) {
        MarketDataInputData::operator=(std::move(_data));
        Client = std::move(_data.Client);
    }
    return *this;
}