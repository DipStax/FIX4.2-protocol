#pragma once

#include <iostream>

#include "Common/Core/Order.hpp"

#define UDP_FLAG_SET_BID(_flag)         _flag |= 01
#define UDP_FLAG_SET_ASK(_flag)         _flag &= ~01
#define UDP_FLAG_GET_BOOK(_flag)        OrderType(_flag & 01)

#define UDP_FLAG_SET_STATUS(_flag, _s)  _flag = (_flag & ~01110) | ((_s & 0111) << 1)
#define UDP_FLAG_GET_STATUS(_flag)      OrderStatus((_flag >> 1) & 0111)

#define UDP_FLAG_SET_SOLD(_flag, _s)    _flag = (_flag & ~010000) | ((_s & 01) << 4)
#define UDP_FLAG_GET_IS_SOLD(_flag)     bool((_flag >> 4) & 01)

#ifndef MARKET_NAME_MAX_SIZE
    #define MARKET_NAME_MAX_SIZE 10
#endif

using Symbol = char [MARKET_NAME_MAX_SIZE];

namespace data
{
    /// @brief Data send to the UDP broadcast in pip::UDPOutNetwork pipeline.
    #pragma pack (push, 1)
    struct UDPPackage
    {
        uint32_t time;
        uint64_t id;
        uint8_t flag;
        Quantity quantity;
        Price price;
        Symbol symbol;
    };
    #pragma pack (pop)

    std::ostream &operator<<(std::ostream &_os, const UDPPackage &_package);
}