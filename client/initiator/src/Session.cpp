#include "Client/Initiator/Session.hpp"

#include "Shared/Log/Manager.hpp"

Session::Session(const std::shared_ptr<net::INetTcp> &_front)
    : m_session_id(GetSessionId()), m_front_socket(_front), Logger(logger::Manager::newLogger("Initiator/Session/" + m_session_id))
{
}

void Session::received(const ipc::Header &_header, net::Buffer &_buffer, Side _side)
{
    switch (_side)
    {
    case Side::Front: handleFrontend(_header, _buffer);
        break;
    default:
        Logger->log<logger::Level::Error>("Unknow side of the message");
        break;
    }
}

std::string Session::GetSessionId()
{
    static size_t id = 0;

    return std::to_string(id++);
}

void Session::handleFrontend(const ipc::Header &_header, net::Buffer &_buffer)
{
    switch (_header.MsgType)
    {
    case ipc::MessageType::Identify:
        
        break;
    default:
        break;
    }
}