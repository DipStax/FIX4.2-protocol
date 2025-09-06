#include "Client/Initiator/Config.hpp"
#include "Client/Initiator/Session.hpp"

#include "Client/Shared/IPC/Helper.hpp"

#include "Shared/Log/Manager.hpp"


Session::Session(const std::shared_ptr<net::INetTcp> &_front)
    : m_session_id(GetSessionId()), Logger(logger::Manager::newLogger("Initiator/Session/" + m_session_id))
{
    m_frontend.socket = _front;
}

void Session::send(const net::Buffer &_buffer, Side _side)
{
    if (_size == Side::Back)
        m_backend.socket->send(_buffer.data(), _buffer.size());
    else
        m_frontend.socket->send(_buffer.data(), _buffer.size());
}

void Session::received(const ipc::Header &_header, net::Buffer &_buffer, Side _side)
{
    switch (_side) {
        case Side::Front: handleFrontend(_header, _buffer);
            break;
        case Side::Back: Logger->log<logger::Level::Warning>("Received new message from backend");
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

std::shared_ptr<net::INetTcp> Session::getFrontSocket() const
{
    return m_frontend.socket;
}

void Session::handleFrontend(const ipc::Header &_header, net::Buffer &_buffer)
{
    Logger->log<logger::Level::Error>("Received new message from frontend: message type: ", (int)_header.MsgType);
    switch (_header.MsgType) {
        case ipc::MessageType::AuthFrontToInitiator: identifyFrontend(_header, _buffer);
            break;
        default: // send reject message
            break;
    }
}

void Session::identifyFrontend(const ipc::Header &_header, net::Buffer &_buffer)
{
    std::ignore = _header;

    Logger->log<logger::Level::Debug>("Identification of the frond end requested");
    if (!m_frontend.apikey_set) {
        ipc::msg::AuthFrontToInitiator authfront{};
        ipc::msg::AuthInitiatorToFront valid_auth{};

        Logger->log<logger::Level::Info>("Frontend identify with info: ", auth);
        _buffer >> auth;
        m_frontend.apikey = auth.apiKey;
        m_frontend.apikey_set = true;
        valid_auth.apikey = m_frontend.apikey;
        send(ipc::Helper::Auth::InitiatorToFront(valid_auth), Side::Front);
        setupBackend(auth);
    } else {
        // send reject message
    }
}

void Session::setupBackend(const ipc::msg::AuthFrontToInitiator &_auth)
{
    Logger->log<logger::Level::Info>("info level");

    m_backend.cmd = shell::Builder()
        .newShellCommand(Configuration<config::Global>::Get().Config.Back.Executable)
        .setEnvironement("FIX42-apikey", _auth.apiKey)
        .setEnvironement("FIX42-initiator-address", Configuration<config::Global>::Get().Config.Back.Address + "." + m_session_id)
        .result();
    Logger->log<logger::Level::Verbose>("New backend setup: ", *(m_backend.cmd));
}