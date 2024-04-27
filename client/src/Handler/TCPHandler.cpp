#include "Client/Handler/TCPHandler.hpp"

namespace io
{
    TCPHandler::TCPHandler(const net::Ip &_ip, uint32_t _port)
    {
        m_socket = std::make_shared<net::tcp::Socket>();
        if (!m_socket->connect(_ip, _port))
            Logger::Log("Restart needed, you are not connected to the server");
        else
            m_selector.client(m_socket);
    }

    void TCPHandler::loop()
    {
        int error = 0;
        std::vector<Client> clients;

        while (*this) {
            clients = m_selector.pull();

            for (Client &_client : clients) {
                fix::Serializer::AnonMessage msg;
                std::string data(_client->receive(MAX_RECV_SIZE, error));

                if (error == 0)
                    continue;
                Logger::Log("TCPHandler: Received: '", data, "'");
                if (fix::Serializer::run(data, msg) != fix::Serializer::Error::None)
                    continue;
                recv(std::move(msg));
            }
            if (!empty(Side::Send)) {
                m_socket->send(pop_front_send().to_string());
            }
        }
    }
}