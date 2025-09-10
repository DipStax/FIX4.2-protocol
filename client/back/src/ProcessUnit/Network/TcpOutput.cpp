#include "Client/Back/ProcessUnit/Network/TcpOutput.hpp"
#include "Client/Back/User.hpp"

#include "Shared/Log/Manager.hpp"

namespace pu
{
    TcpOutputNetwork::TcpOutputNetwork(Socket _server)
        : AInputProcess<InputType>("Back/NET/TCP-Output"),
        m_server(_server)
    {
    }

    void TcpOutputNetwork::onInput(InputType _input)
    {
        _input.header.set34_msgSeqNum(std::to_string(User::Instance().getSeqNumber()));
        _input.header.set49_SenderCompId(User::Instance().getUserId());
        _input.header.set56_TargetCompId(PROVIDER_NAME);
        User::Instance().nextSeqNumber();

        std::string data = _input.to_string();

        if (m_server->send(data) == 0)
            Logger->log<logger::Level::Error>("Failed to send message to server: ", data);
        else
            Logger->log<logger::Level::Info>("Sended new message: ", data);
    }
}