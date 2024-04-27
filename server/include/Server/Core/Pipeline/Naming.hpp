#pragma once

#include "Common/Message/Fix.hpp"
#include "Common/Message/Serializer.hpp"
#include "Common/Thread/Queue.hpp"
#include "Server/Core/OrderBook.hpp"
#include "Server/Core/ClientSocket.hpp"
#include "Common/Network/UDPPackage.hpp"

/// @brief Data transfered from the pip::InNetwork pipeline to the pip::Action pipeline.
struct ActionInput
{
    ActionInput() = default;
    ActionInput(const ActionInput &&_data) noexcept;
    ActionInput(const ClientSocket &_client, const fix::Serializer::AnonMessage &&_message);

    ActionInput &operator=(ActionInput &&_data) noexcept;

    ClientSocket Client{};                      ///< Sender client information.
    fix::Serializer::AnonMessage Message{};     ///< Undefined message data.
};

/// @brief Data transfered from the pip::Action pipeline to the pip::Market pipeline
struct MarketInput
{
    MarketInput() = default;
    MarketInput(const MarketInput &&_data) noexcept;
    MarketInput(const MarketInput &_data);
    MarketInput(const ClientSocket &&_client) noexcept;

    MarketInput &operator=(MarketInput &&_data) noexcept;

    ClientSocket Client{};                              ///< Sender client information.
    OrderBook::Data OrderData{};                        ///< Action to apply to the OrderBook.
};

/// @brief Data transfered from the pip::Market pipeline to the pip::OutNetwork pipeline
struct OutNetworkInput
{
    OutNetworkInput() = default;
    OutNetworkInput(const OutNetworkInput &&_data) noexcept;
    OutNetworkInput(const OutNetworkInput &_data);
    OutNetworkInput(const ClientSocket &&_client, const fix::Message &&_msg) noexcept;
    OutNetworkInput(const ClientSocket &_client, const fix::Message &_msg);

    OutNetworkInput &operator=(OutNetworkInput &&_data) noexcept;

    ClientSocket Client{};                              ///< Sender client information.
    fix::Message Message{};                     ///< Final message send to the client.
};

struct MarketDataInputData
{
    std::string Id;
    uint8_t SubType;
    uint8_t Depth;
    uint8_t UpdateType;
    std::vector<OrderType> Types;
    std::vector<std::string> Symbols;
};

struct MarketDataInput : public MarketDataInputData
{
    MarketDataInput() = default;
    MarketDataInput(const MarketDataInput &&_data) noexcept;
    MarketDataInput(const MarketDataInputData &&_data, const ClientSocket &&_client) noexcept;

    MarketDataInput &operator=(MarketDataInput &&_data) noexcept;

    ClientSocket Client{};
};

/// @brief Queue type use to transfer data from pip::InNetwork to pip::Action pipeline.
using InAction = ts::Queue<ActionInput>;
/// @brief Queue type use to transfer data from pip::Action to pip::Market pipeline.
using InMarket = ts::Queue<MarketInput>;
/// @brief Queue type use to transfer data from pip::Market to pip::OutNetwork pipeline.
using InOutNetwork = ts::Queue<OutNetworkInput>;
/// @brief Queue type use to transfer direct message from any pipeline to the pip::OutNetwork pipeline.
using InMarketData = ts::Queue<MarketDataInput>;
/// @brief Queue type use to transfer data to be formated and send by the pip::UDPOutNetwork pipeline.
using InUDP = ts::Queue<data::UDPPackage>;

/// @brief Map of market input MarketInput with as key the symbol of the market.
using MarketEntry = std::unordered_map<std::string, InMarket &>;