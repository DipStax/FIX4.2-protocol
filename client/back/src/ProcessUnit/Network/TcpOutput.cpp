#include "Client/Back/ProcessUnit/Network/TcpOutput.hpp"
#include "Client/Back/User.hpp"
#include "Client/Back/Config.hpp"

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
        fix42::Header header{};

        if (!User::Instance().isLogin())
            User::Instance().nextSeqNumber();

        header.getPositional<fix42::tag::BeginString>().Value = "FIX.4.2";
        header.getPositional<fix42::tag::BodyLength>().Value = _input.Message.size();
        header.getPositional<fix42::tag::MsgType>().Value = _input.MessageType;
        header.get<fix42::tag::MsgSeqNum>().Value = User::Instance().getSeqNumber();
        header.get<fix42::tag::SenderCompId>().Value = User::Instance().getUserId();
        header.get<fix42::tag::TargetCompId>().Value = Configuration<config::Global>::Get().Config.FixServer.ProviderName;
        header.get<fix42::tag::SendingTime>().Value = std::chrono::system_clock::now();
        Logger->log<logger::Level::Verbose>("Header info filled");

        User::Instance().nextSeqNumber();

        std::string data = header.to_string() + _input.Message;

        AddCheckSum(data);

        if (m_server->isOpen()) {
            if (!m_server->send(reinterpret_cast<const std::byte *>(data.c_str()), data.size())) {
                Logger->log<logger::Level::Error>("Unable to send messsage to server: ", strerror(errno));
            } else {
                Logger->log<logger::Level::Info>("Message send successfuly: ", data);
            }
        } else {
            Logger->log<logger::Level::Fatal>("Client not connected but still present, removing from server");
        }
    }

    void TcpOutputNetwork::AddCheckSum(std::string &_msg)
    {
        uint32_t checksum = 0;

        for (char _c : _msg)
            checksum += _c;
        _msg += "10=" + std::to_string(static_cast<uint8_t>(checksum % 256)) + "\1";
    }
}