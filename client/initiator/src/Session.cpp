#include <pqxx/pqxx>

#include "Client/Initiator/Config.hpp"
#include "Client/Initiator/Session.hpp"

#include "Client/Shared/IPC/Helper.hpp"

#include "Shared/Log/Manager.hpp"
#include "Shared/PgSQL/ConnectionPool.hpp"
#include "Shared/Utils/Utils.hpp"

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
    return utils::Uuid::Generate();
}

const std::string &Session::getId() const
{
    return m_session_id;
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

void Session::close()
{
    {
        sql::Connection &conn = sql::ConnectionPool<1>::GetAvailableConnection();
        pqxx::work tnx_update{*(conn.Conn)};
        (void)tnx_update.exec("UPDATE dev.client SET connected = $1 WHERE apikey = $2 AND client_name = $3", pqxx::params{false, m_frontend.apikey, m_frontend.name});

        tnx_update.commit();
        conn.done();
    }
    if (m_backend.cmd != nullptr)
        m_backend.cmd->stop();
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

        std::optional<std::string> server_name = login(authfront.apikey, authfront.name);

        if (!server_name.has_value())
            return;
        m_frontend.name = server_name.value();
        m_frontend.apikey = authfront.apikey;
        valid_auth.name = m_frontend.name;
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

std::optional<std::string> Session::login(const std::string &_apikey, const std::string &_name)
{
    pqxx::result result{};

    {
        sql::Connection &conn = sql::ConnectionPool<1>::GetAvailableConnection();
        pqxx::read_transaction tnx_select{*(conn.Conn)};
        result = tnx_select.exec("SELECT connected, server_name, apikey, client_name FROM dev.client WHERE apikey = $1 AND client_name = $2 LIMIT 1", pqxx::params{_apikey, _name});

        tnx_select.commit();
        conn.done();
    }
    if (result.empty()) {
        Logger->log<logger::Level::Warning>("User trying to connect not found: ", _name);
        ipc::msg::Reject reject{"No user found"};
        send(ipc::Helper::Reject(reject), Side::Front);
        return std::nullopt;
    }
    Logger->log<logger::Level::Info>("User validated from database");

    auto [connect, server_name, _, __] = result.at(0).as<bool, std::string, std::string, std::string>();

    if (connect) {
        Logger->log<logger::Level::Warning>("User trying to reconnect: ", _name);
        ipc::msg::Reject reject{"User already logged in"};
        send(ipc::Helper::Reject(reject), Side::Front);
        return std::nullopt;
    }
    {
        sql::Connection &conn = sql::ConnectionPool<1>::GetAvailableConnection();
        pqxx::work tnx_update{*(conn.Conn)};
        (void)tnx_update.exec("UPDATE dev.client SET connected = $1 WHERE apikey = $2 AND client_name = $3", pqxx::params{true, _apikey, _name});

        tnx_update.commit();
        conn.done();
    }
    Logger->log<logger::Level::Info>("User set as connected in the database");
    return server_name;
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