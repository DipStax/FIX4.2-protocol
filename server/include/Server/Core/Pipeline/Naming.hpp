#pragma once

#include "Common/Message/Fix.hpp"
#include "Common/Message/Serializer.hpp"
#include "Common/Thread/Queue.hpp"
#include "Server/Core/OrderBook.hpp"
#include "Common/Network/UDPPackage.hpp"
#include "Server/Core/ClientStore.hpp"

template<class T>
struct Context : T
{
    Context() = default;
    Context(const Context<T> &&_data) noexcept;
    Context(const Context<T> &_data);
    template<class ...Ts>
    Context(const ClientStore::Client &_client, std::chrono::system_clock::time_point _time, Ts &&..._args);
    ~Context() = default;

    ClientStore::Client Client = nullptr;                      ///< Sender client information.
    std::chrono::system_clock::time_point ReceiveTime;

    Context<T> &operator=(Context<T> &&_data) noexcept;
};

/// @brief Data transfered from the pip::InNetwork pipeline to the pip::Action pipeline.
struct RouterInput
{
    RouterInput() = default;
    RouterInput(RouterInput &&_data) noexcept;
    RouterInput(const RouterInput &_data);
    RouterInput(const fix::Serializer::AnonMessage &&_message);
    virtual ~RouterInput() = default;

    RouterInput &operator=(RouterInput &&_data) noexcept;

    fix::Serializer::AnonMessage Message{};     ///< Undefined message data.
};


struct MarketRefreshInputData
{
    std::string Id;
    uint8_t SubType;
    uint8_t Depth;
    uint8_t UpdateType;
    std::vector<OrderType> Types;
};

/// @brief Data transfered from the pip::Action pipeline to the pip::Market pipeline
struct MarketInput
{
    MarketInput() = default;
    MarketInput(MarketInput &&_data) noexcept;
    MarketInput(const MarketInput &_data);
    virtual ~MarketInput();

    enum Type
    {
        Order,
        Refresh
    };

    MarketInput &operator=(MarketInput &&_data) noexcept;

    Type type = Order;

    union {
        OrderBook::Data OrderData{};                        ///< Action to apply to the OrderBook.
        MarketRefreshInputData RefreshData;
    };
};

/// @brief Data transfered from the pip::Market pipeline to the pip::OutNetwork pipeline
struct OutNetworkInput
{
    OutNetworkInput() = default;
    OutNetworkInput(OutNetworkInput &&_data) noexcept;
    OutNetworkInput(const OutNetworkInput &_data);
    OutNetworkInput(const fix::Message &&_msg) noexcept;
    virtual ~OutNetworkInput() = default;

    OutNetworkInput &operator=(OutNetworkInput &&_data) noexcept;

    fix::Message Message{};                     ///< Final message send to the client.
};

/// @brief Queue type use to transfer data from pip::InNetwork to pip::Action pipeline.
using InputRouter = ts::Queue<Context<RouterInput>>;
/// @brief Queue type use to transfer data from pip::Action to pip::Market pipeline.
using InMarket = ts::Queue<Context<MarketInput>>;
/// @brief Queue type use to transfer data from pip::Market to pip::OutNetwork pipeline.
using InOutNetwork = ts::Queue<Context<OutNetworkInput>>;
/// @brief Queue type use to transfer data to be formated and send by the pip::UDPOutNetwork pipeline.
using InUDP = ts::Queue<data::UDPPackage>;

/// @brief Map of market input MarketInput with as key the symbol of the market.
using MarketEntry = std::unordered_map<std::string, InMarket &>;

#include "Server/Core/Pipeline/Naming.inl"