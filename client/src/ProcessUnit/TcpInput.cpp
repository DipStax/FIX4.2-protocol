#include "Client/ProcessUnit/TcpInput.hpp"

#include "Common/Log/Manager.hpp"
#include "Common/Message/Reject.hpp"

namespace pu
{
    TcpInputNetwork::TcpInputNetwork(TcpInputNetwork::Socket _server, QueueTransit &_output)
        : m_server(_server), m_output(_output), Logger(log::Manager::newLogger("TCP-Input"))
    {
        m_selector.client(m_server);
        m_selector.timeout(1.0);
        Logger->log<log::Level::Verbose>("Server socket added to selector");
    }

    void TcpInputNetwork::runtime(std::stop_token _st)
    {
        Logger->log<log::Level::Info>("Starting process unit...");
        int error = 0;

        while (!_st.stop_requested()) {
            std::vector<Socket> clients = m_selector.pull();

            if (!clients.empty()) {
                std::string data = m_server->receive(MAX_RECV_SIZE, error);
                fix::Serializer::AnonMessage msg{};

                if (error == 0) {
                    Logger->log<log::Level::Fatal>("Server socket disonnected");
                    // todo
                    continue;
                }
                if (fix::Serializer::run(data, msg) != fix::Serializer::Error::None) {
                    fix::Reject reject;

                    // build reject
                    // m_error.append(_client, std::chrono::system_clock::now(), std::move(reject));
                    Logger->log<log::Level::Error>("Message deserialization failed");
                } else {
                    Logger->log<log::Level::Info>("New serialized message received");
                    m_output.push(std::move(msg));
                }
            }
        }
    }
}