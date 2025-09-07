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
    if (_side == Side::Back)
        m_backend.socket->send(_buffer.data(), _buffer.size());
    else
        m_frontend.socket->send(_buffer.data(), _buffer.size());
}

void Session::received(const ipc::Header &_header, net::Buffer &_buffer, Side _side)
{
    switch (_side) {
        case Side::Front: handleFrontend(_header, _buffer);
            break;
        case Side::Back: handleBackend(_header, _buffer);
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

std::shared_ptr<net::UnixStream> Session::getBackSocket() const
{
    return m_backend.socket;
}

const std::string &Session::getApiKey() const
{
    if (!m_frontend.apikey.has_value())
        throw std::runtime_error("Api key not set");
    return m_frontend.apikey.value();
}

std::string Session::GenerateToken()
{
    static size_t token = 0;

    return std::to_string(token);
}

void Session::handleFrontend(const ipc::Header &_header, net::Buffer &_buffer)
{
    Logger->log<logger::Level::Error>("Received new message from frontend: message type: ", (int)_header.MsgType);
    switch (_header.MsgType) {
        case ipc::MessageType::FrontToInitiatorAuth: identifyFrontend(_buffer);
            break;
        default: // send reject message
            break;
    }
}

void Session::identifyFrontend(net::Buffer &_buffer)
{
    Logger->log<logger::Level::Debug>("Identification of the frond end requested");
    if (!m_frontend.apikey.has_value()) {
        ipc::msg::AuthFrontToInitiator authfront{};
        ipc::msg::AuthInitiatorToFront valid_auth{};

        Logger->log<logger::Level::Verbose>("API Key not set, verifying authentification");
        _buffer >> authfront;
        Logger->log<logger::Level::Info>("Frontend identify with info: ", authfront);
        m_frontend.apikey = authfront.apikey;
        valid_auth.apikey = authfront.apikey;
        send(ipc::Helper::Auth::InitiatorToFront(valid_auth), Side::Front);
        setupBackend(valid_auth);
    } else {
        Logger->log<logger::Level::Error>("Frontend try to reauth with the initiator");
    }
}

void Session::setupBackend(const ipc::msg::AuthFrontToInitiator &_auth)
{
    m_backend.cmd = shell::Builder()
        .newShellCommand(Configuration<config::Global>::Get().Config.Back.Executable)
        .setEnvironement("FIX42-apikey", _auth.apikey)
        .setEnvironement("FIX42-initiator-address", Configuration<config::Global>::Get().Config.Back.Address)
        .result();
    Logger->log<logger::Level::Verbose>("New backend setup: ", *(m_backend.cmd));
    m_backend.cmd->run();
    Logger->log<logger::Level::Info>("New process created for backend, pid: ", m_backend.cmd->getPID());
}

void Session::handleBackend(const ipc::Header &_header, net::Buffer &_buffer)
{
    switch (_header.MsgType) {
        case ipc::MessageType::BackToInitiatorAuth: identifyBackend(_buffer);
            break;
        default: // send reject message
            break;
    }
}

void Session::identifyBackend(net::Buffer &_buffer)
{
    ipc::msg::AuthBackToInitiator auth;
    ipc::msg::AuthInitiatorToBack auth_valid;

    _buffer >> auth;
    auth_valid.apikey = auth.apikey;
    auth_valid.token = GenerateToken();
    send(ipc::Helper::Auth::InitiatorToBack(auth_valid), Side::Back);
}