#include <future>

#include "Server/Core/ProcessUnit/Network/TCPOutput.hpp"
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
            _input.Message.header.set34_msgSeqNum(std::to_string(_input.Client->getSeqNumber()));
            _input.Message.header.set49_SenderCompId(PROVIDER_NAME);
            _input.Message.header.set56_TargetCompId(_input.Client->getUserId());
            _input.Client->nextSeqNumber();

            std::string data = _input.Message.to_string();

            if (_input.Client->getSocket()) { // todo is_open
                if (_input.Client->getSocket()->send(reinterpret_cast<const std::byte *>(data.c_str()), data.size()) == data.size())
                    Logger->log<logger::Level::Info>("[Responce] Data send successfuly: ", data);
                else
                    Logger->log<logger::Level::Error>("[Responce] Error occured when sending data");
                // todo log timing
                if (_input.Client->shouldDisconnect()) {
                    _input.Client->disconnect();
                    ClientStore::Instance().removeClient(_input.Client);
                    Logger->log<logger::Level::Debug>("[Responce] Client has been disconnected: ", _input.Client->getUserId());
                }
            } else {
                Logger->log<logger::Level::Warning>("[Responce] Client not connected: ", _input.Client->getUserId());
                ClientStore::Instance().removeClient(_input.Client);
                return true;
            }
            return false;
        });
    }
}