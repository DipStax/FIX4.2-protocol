#include "Client/Back/ProcessUnit/Network/TcpInput.hpp"

#include "Common/Log/Manager.hpp"
#include "Common/Message/Reject.hpp"

namespace pu
{
    TcpInputNetwork::TcpInputNetwork(TcpInputNetwork::Socket _server, QueueTransit &_output)
        : AProcessUnitBase("Client/NET/TCP-Input"),
        m_server(_server), m_output(_output)
    {
        m_selector.timeout(1000);
        Logger->log<logger::Level::Verbose>("Server socket added to selector");
    }

    void TcpInputNetwork::runtime(std::stop_token _st)
    {
        Logger->log<logger::Level::Info>("Launching process unit runtime");
        int error = 0;

        m_selector.client(m_server);
        while (!_st.stop_requested()) {
            std::vector<Socket> clients = m_selector.pull();

            if (!clients.empty()) {
                std::vector<std::byte> bytes = clients[0]->receive(MAX_RECV_SIZE, error);
                std::string data(reinterpret_cast<const char*>(bytes.data()), + bytes.size());
                fix::Serializer::AnonMessage msg{};

                if (error == 0) {
                    Logger->log<logger::Level::Fatal>("Server socket disonnected");
                    // todo
                    continue;
                }
                Logger->log<logger::Level::Info>("New message received: ", data);
                if (fix::Serializer::run(data, msg) != fix::Serializer::Error::None) {
                    fix::Reject reject;

                    // build reject
                    // m_error.append(_client, std::chrono::system_clock::now(), std::move(reject));
                    Logger->log<logger::Level::Error>("Message deserialization failed");
                } else {
                    m_output.push(std::move(msg));
                }
            }
        }
    }
}