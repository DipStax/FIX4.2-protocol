#include "Server/Core/InternalClient.hpp"

InternalClient::InternalClient(std::shared_ptr<net::tcp::Socket> _socket)
    : m_socket(_socket)
{
}

InternalClient::InternalClient(const InternalClient &_client)
    : Logged(_client.Logged), Disconnect(_client.Disconnect),
        User(_client.User), SeqNumber(_client.SeqNumber),
        ClientSeqNumber(_client.ClientSeqNumber),
        m_socket(_client.m_socket), m_request(_client.m_request),
        m_subscribe(_client.m_subscribe)
{
}

InternalClient::InternalClient(const InternalClient &&_client) noexcept
    : Logged(std::move(_client.Logged)), Disconnect(std::move(_client.Disconnect)),
        User(std::move(_client.User)), SeqNumber(std::move(_client.SeqNumber)),
        ClientSeqNumber(std::move(_client.ClientSeqNumber)),
        m_socket(std::move(_client.m_socket)), m_request(std::move(_client.m_request)),
        m_subscribe(std::move(_client.m_subscribe))
{
}

std::shared_ptr<net::tcp::Socket> InternalClient::getSocket() const
{
    return m_socket;
}

void InternalClient::newRequest()
{
    m_request.emplace(SeqNumber, std::chrono::system_clock::now());
    ClientSeqNumber++;
}

bool InternalClient::hasRequest(size_t _seqNumber) const
{
    return m_request.contains(_seqNumber);
}

std::chrono::system_clock::time_point InternalClient::getRequest(size_t _seqNumber)
{
    auto it = m_request.find(_seqNumber);

    if (it == m_request.end())
        return std::chrono::system_clock::now();
    auto ret = it->second;
    m_request.erase(it);
    return ret;
}

InternalClient::Subs &InternalClient::subscribe(const std::string &_symbol)
{
    return m_subscribe[_symbol];
}

void InternalClient::unsubscribe(const std::string &_symbol)
{
    m_subscribe.erase(_symbol);
}

InternalClient &InternalClient::operator=(InternalClient &&_client) noexcept
{
    if (this != &_client) {
        Logged = std::move(_client.Logged);
        Disconnect = std::move(_client.Disconnect);
        User = std::move(_client.User);
        SeqNumber = std::move(_client.SeqNumber);
        ClientSeqNumber = std::move(_client.ClientSeqNumber);
        m_socket = std::move(_client.m_socket);
    }
    return *this;
}

bool InternalClient::operator==(const InternalClient &_client) const
{
    return m_socket == _client.m_socket && Logged == _client.Logged && User == _client.User;
}

InternalClient::operator bool() const
{
    return m_socket->is_open();
}

std::ostream &operator<<(std::ostream &_os, const InternalClient &_client)
{
    _os << "InternalClient: { User: " << _client.User << ", Logged: " << _client.Logged << ", Disconnect: " << _client.Disconnect;
    _os << ", SeqNumber: " << _client.SeqNumber << ", ClientSeqNumber: " << _client.ClientSeqNumber << " }";
    return _os;
}