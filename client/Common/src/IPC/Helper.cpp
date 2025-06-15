#include "Client/Common/IPC/Header.hpp"
#include "Client/Common/IPC/Helper.hpp"

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

    net::Buffer Helper::Logon(const msg::Logon &_logon)
    {
        net::Buffer buffer;
        ipc::Header header{
            ipc::MessageType::Logon,
            sizeof(uint32_t) * 2 + _logon.UserId.size() + sizeof(float)
        };

        buffer << header << _logon;
        return buffer;
    }
}