#include <pqxx/pqxx>

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
    Logger->log<logger::Level::Info>("Received new message from session");
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

void Session::setBackendSocket(const std::shared_ptr<net::UnixStream> &_socket)
{
    m_backend.socket = _socket;
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
        Logger->log<logger::Level::Debug>("Sending auth validation to frontend: ", valid_auth);
        send(ipc::Helper::Auth::InitiatorToFront(valid_auth), Side::Front);
        buildShellBack();
        Logger->log<logger::Level::Verbose>("New backend setup: ", *(m_backend.cmd));
        m_backend.cmd->run();
        Logger->log<logger::Level::Info>("New process created for backend, pid: ", m_backend.cmd->getPID());
    } else {
        Logger->log<logger::Level::Error>("Frontend try to reauth with the initiator");
    }
}

void Session::buildShellBack()
{
    shell::Builder builder{};

    builder.newShellCommand(Configuration<config::Global>::Get().Config.Back.Executable)
        .setEnvironement("FIX42-apikey", m_frontend.apikey.value())
        .setEnvironement("FIX42-initiator-address", Configuration<config::Global>::Get().Config.Back.Address);

    for (const std::string &_arg : Configuration<config::Global>::Get().Config.Back.ExecArgs)
        builder.addArgument(_arg);
    m_backend.cmd = builder.result();
}

void Session::handleBackend(const ipc::Header &_header, net::Buffer &_buffer)
{
    Logger->log<logger::Level::Error>("Received new message from backend: message type: ", (int)_header.MsgType);
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
    ipc::msg::AuthInitiatorToBack valid_auth;

    Logger->log<logger::Level::Verbose>("Identiying backend");
    _buffer >> auth;
    Logger->log<logger::Level::Info>("Backend authenticated with info: ", auth);
    valid_auth.apikey = auth.apikey;
    m_backend.token = GenerateToken();
    valid_auth.token = m_backend.token.value();
    Logger->log<logger::Level::Debug>("Sending auth validation to backend: ", valid_auth);
    send(ipc::Helper::Auth::InitiatorToBack(valid_auth), Side::Back);

    ipc::msg::InitiatorToFrontValidToken validation_token{valid_auth.token, auth.port};

    Logger->log<logger::Level::Debug>("Sending connection info to frontend: ", validation_token);
    send(ipc::Helper::ValidationToken::InitiatorToFront(validation_token), Side::Front);
}