#include "Common/Network/Ip.hpp"

namespace net
{
    template<class ...Args>
    Ip::Ip(Args &&..._args)
    {
        set(std::forward<Args>(_args)...);
    }
}