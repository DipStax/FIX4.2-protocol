#include "Client/Back/ProcessUnit/Network/TcpOutput.hpp"
#include "Client/Back/User.hpp"

#include "Common/Log/Manager.hpp"

namespace pu
{
    TcpOutputNetwork::TcpOutputNetwork(Socket _server)
        : m_server(_server), Logger(logger::Manager::newLogger("NET/TCP-Output"))
    {
    }

    TcpOutputNetwork::QueueInputType &TcpOutputNetwork::getInput()
    {
        return m_input;
    }

    void TcpOutputNetwork::runtime(std::stop_token _st)
    {
        Logger->log<logger::Level::Info>("Starting process unit...");
        InputType input;

        while (!_st.stop_requested()) {
            while (!m_input.empty()) {
                input = m_input.pop_front();

                input.header.set34_msgSeqNum(std::to_string(User::Instance().nextSeqNumber()));
                input.header.set49_SenderCompId(User::Instance().getUserId());
                input.header.set56_TargetCompId(PROVIDER_NAME);

                std::string data = input.to_string();

                if (m_server->send(data) == 0)
                    Logger->log<logger::Level::Error>("Failed to send message to server: ", data);
                else
                    Logger->log<logger::Level::Info>("Sended new message: ", data);
            }
        }
    }
}