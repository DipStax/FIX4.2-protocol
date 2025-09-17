#include "Server/InternalClient.hpp"

InternalClient::InternalClient(std::shared_ptr<net::INetTcp> _socket)
    : m_socket(_socket)
{
}

void InternalClient::login(const std::string &_user_id)
{
    m_logged_in = true;
    m_user_id = _user_id;
}

bool InternalClient::isLoggedin() const
{
    return m_logged_in;
}

std::string InternalClient::getUserId() const
{
    return m_user_id;
}

bool InternalClient::isConnected() const
{
    return m_socket != nullptr && m_socket->isOpen();
}

bool InternalClient::close()
{
    if (m_socket == nullptr)
        return false;
    return m_socket->close();
}

void InternalClient::disconnect()
{
    m_user_id = "";
    m_logged_in = false;
    m_should_dc = false;
    m_seq_num = 0;
}

void InternalClient::shouldDisconnect(bool _disconnect)
{
    m_should_dc = _disconnect;
}

bool InternalClient::shouldDisconnect() const
{
    return m_should_dc;
}

void InternalClient::setSeqNumber(uint32_t _seq_num)
{
    m_seq_num = _seq_num;
}

void InternalClient::nextSeqNumber()
{
    m_seq_num++;
}

uint32_t InternalClient::getSeqNumber() const
{
    return m_seq_num;
}

bool InternalClient::send(const std::byte *_data, size_t _len)
{
    return m_socket->send(_data, _len) != -1;
}

std::shared_ptr<net::INetTcp> InternalClient::getSocket() const
{
    return m_socket;
}

InternalClient::HeartBeatInfo &InternalClient::getHeartBeatInfo()
{
    return m_hb_info;
}

bool InternalClient::operator==(const InternalClient &_client) const
{
    return m_socket == _client.m_socket && m_logged_in == _client.m_logged_in && m_user_id == _client.m_user_id;
}

std::ostream &operator<<(std::ostream &_os, const InternalClient &_client)
{
    _os << "InternalClient: { User: " << _client.m_user_id << ", Logged: " << _client.m_logged_in << ", ShouldDisconnect: " << _client.m_should_dc;
    _os << ", SeqNumber: " << _client.m_seq_num << " }";
    return _os;
}