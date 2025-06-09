#include <future>

#include "Server/Core/ProcessUnit/Network/TCPOutput.hpp"
#include "Common/Log/Manager.hpp"

namespace pu
{
    TcpOutputNetwork::TcpOutputNetwork()
        : Logger(log::Manager::newLogger("TCP-Output"))
    {
    }

    TcpOutputNetwork::QueueInputType &TcpOutputNetwork::getInput()
    {
        return m_input;
    }

    void TcpOutputNetwork::runtime(std::stop_token _st)
    {
        Logger->log<log::Level::Info>("Starting process unit...");
        InputType input;

        while (!_st.stop_requested()) {
            while (!this->m_input.empty()) {
                m_tp.enqueue([this, _input = std::move(m_input.pop_front())] () mutable {
                    _input.Message.header.set34_msgSeqNum(std::to_string(_input.Client->nextSeqNumber()));
                    _input.Message.header.set49_SenderCompId(PROVIDER_NAME);
                    _input.Message.header.set56_TargetCompId(_input.Client->getUserId());

                    std::string data = _input.Message.to_string();

                    if (_input.Client->getSocket()) { // todo is_open
                        if (_input.Client->getSocket()->send(reinterpret_cast<const std::byte *>(data.c_str()), data.size()) == data.size())
                            Logger->log<log::Level::Info>("[Responce] Data send successfuly: ", data);
                        else
                            Logger->log<log::Level::Error>("[Responce] Error occured when sending data");
                        // todo log timing
                        Logger->log<log::Level::Info>("[Responce] Updated client status: { UserId: ", _input.Client->getUserId(), " }"); // todo log
                        if (_input.Client->shouldDisconnect()) {
                            _input.Client->disconnect();
                            ClientStore::Instance().removeClient(_input.Client);
                            Logger->log<log::Level::Debug>("[Responce] Client has been disconnected: ", _input.Client->getUserId());
                        }
                    } else {
                        Logger->log<log::Level::Warning>("[Responce] Client not connected: ", _input.Client->getUserId());
                        ClientStore::Instance().removeClient(_input.Client);
                        return true;
                    }
                    return false;
                });
            }
        }
        Logger->log<log::Level::Warning>("Exiting process unit...", _st.stop_requested());
    }
}