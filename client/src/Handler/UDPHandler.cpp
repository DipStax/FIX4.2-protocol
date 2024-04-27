#include "Client/Handler/UDPHandler.hpp"

namespace io
{
    UDPHandler::UDPHandler(const net::Ip &_ip, uint32_t _port)
        : m_ip(_ip)
    {
        m_socket = std::make_shared<net::udp::Socket>();
        m_socket->connect(_ip, _port);
        m_socket->blocking(false);
        m_selector.timeout(100);
    }

    uint32_t UDPHandler::port() const
    {
        return m_socket->getPort();
    }

    net::Ip UDPHandler::ip() const
    {
        return m_ip;
    }

    void UDPHandler::loop()
    {
        m_selector.client(m_socket);
        std::vector<Client> clients;
        int error = 0;

        while (*this) {
            clients = m_selector.pull();
            if (!clients.empty()) {
                const data::UDPPackage *package = reinterpret_cast<const data::UDPPackage *>(m_socket->receiveUDP(sizeof(data::UDPPackage), error).c_str());

                recv(std::move(*package));
            }
            if (!empty(io::Side::Send)) {
                data::UDPPackage package = pop_front_send();

                m_socket->send(reinterpret_cast<const uint8_t *>(&package), sizeof(data::UDPPackage));
            }
        }
    }
}