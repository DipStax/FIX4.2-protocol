#pragma once

#include "Common/Network/UDPPackage.hpp"
#include "Common/Core/Order.hpp"
#include "Client/History.hpp"

struct Subscribtion
{
    std::string Id;
    std::vector<Symbol> Symbols;
    std::vector<OrderType> Type;
    size_t Depth;
};

struct Context
{
    bool Loggin = false;
    UserId User;
    uint32_t HeartBit = 0;
    uint32_t SeqNum = 1;
    History userInfos;

    std::vector<Subscribtion> SubsStaging;
    std::vector<Subscribtion> Subs;
};

std::ostream &operator<<(std::ostream &_os, const Context &_ctx);