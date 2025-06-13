#include "Client/Common/ipc/Header.hpp"
#include "Client/Common/ipc/Helper.hpp"

namespace ipc
{
    net::Buffer Helper::Status(PUStatus _status)
    {
        net::Buffer buffer;
        ipc::Header header{
            ipc::MessageType::Status,
            sizeof(PUStatus)
        };

        return buffer << header << static_cast<uint8_t>(_status);
    }

    net::Buffer Helper::Logon(const std::string &_name, uint32_t _seqnum)
    {
        net::Buffer buffer;
        ipc::Header header{
            ipc::MessageType::Logon,
            sizeof(uint32_t) * 2 + _name.size()
        };

        buffer << header << _name << _seqnum;
        return buffer;
    }
}