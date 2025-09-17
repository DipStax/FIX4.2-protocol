#include <future>

#include "Server/ProcessUnit/Network/TCPOutput.hpp"
#include "Server/Config.hpp"

#include "Shared/Log/Manager.hpp"

namespace pu
{
    TcpOutputNetwork::TcpOutputNetwork()
        : AInputProcess<InputType>("Server/NET/TCP-Output")
    {
    }

    void TcpOutputNetwork::onInput(InputType _input)
    {
        m_tp.enqueue([this, _input] () mutable {
            fix42::Header header{};

            header.getPositional<fix42::tag::BeginString>().Value = "FIX.4.2";
            header.getPositional<fix42::tag::BodyLength>().Value = _input.Message.size();
            header.getPositional<fix42::tag::MsgType>().Value = _input.MessageType;
            header.get<fix42::tag::MsgSeqNum>().Value = _input.Client->getSeqNumber();
            header.get<fix42::tag::SenderCompId>().Value = Configuration<config::Global>::Get().Config.Fix.ProviderName;
            header.get<fix42::tag::TargetCompId>().Value = _input.Client->getUserId();
            header.get<fix42::tag::SendingTime>().Value = std::chrono::system_clock::now();
            Logger->log<logger::Level::Verbose>("Header info filled");

            _input.Client->nextSeqNumber();

            std::string data = header.to_string() + _input.Message;

            AddCheckSum(data);

            if (_input.Client->isConnected()) {
                if (!_input.Client->send(reinterpret_cast<const std::byte *>(data.c_str()), data.size()) == data.size()) {
                    Logger->log<logger::Level::Error>("Unable to send messsage to client: ", _input.Client->getUserId());
                    ClientStore::Instance().removeClient(_input.Client);
                    Logger->log<logger::Level::Verbose>("Removing client session from the server, requested");
                } else {
                    Logger->log<logger::Level::Info>("Message send successfuly: ", data);
                }
                if (_input.Client->shouldDisconnect()) {
                    ClientStore::Instance().removeClient(_input.Client);
                    Logger->log<logger::Level::Debug>("Client has been disconnected: ", _input.Client->getUserId());
                }
            } else {
                Logger->log<logger::Level::Warning>("Client not connected but still present, removing from server");
                ClientStore::Instance().removeClient(_input.Client);
            }
        });
    }

    void TcpOutputNetwork::AddCheckSum(std::string &_msg)
    {
        uint32_t checksum = 0;

        for (char _c : _msg)
            checksum += _c;
        _msg += "10=" + std::to_string(static_cast<uint8_t>(checksum % 256)) + "\1";
    }
}