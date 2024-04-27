#include "Common/Network/UDPPackage.hpp"

namespace data
{
    std::ostream &operator<<(std::ostream &_os, const UDPPackage &_packeage)
    {
        _os << "{ id: " << _packeage.id <<  ", prcie: " << _packeage.price << ", quantity: " << _packeage.quantity;
        _os << ", book: " << ((UDP_FLAG_GET_BOOK(_packeage.flag) == OrderType::Ask) ? "Ask" : "Bid");
        _os << ", status: " << UDP_FLAG_GET_STATUS(_packeage.flag);
        _os << ", is-sold: " << UDP_FLAG_GET_IS_SOLD(_packeage.flag) << " }";
        return _os;
    }
}