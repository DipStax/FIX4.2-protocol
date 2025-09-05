#include "Client/Initiator/Config.hpp"
#include "Client/Initiator/Session.hpp"

#include "Client/Shared/IPC/Helper.hpp"

#include "Shared/Log/Manager.hpp"


Session::Session(const std::shared_ptr<net::INetTcp> &_front)
    : m_session_id(GetSessionId()), Logger(logger::Manager::newLogger("Initiator/Session/" + m_session_id))
{
    m_frontend.socket = _front;
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
        case ipc::MessageType::Identify: identifyFrontend(_header, _buffer);
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
        ipc::msg::IdentifyFront identify;
        net::Buffer reply = ipc::Helper::FrontIdentification(identify);

        _buffer >> identify;
        m_frontend.apikey = identify.apiKey;
        m_frontend.apikey_set = true;
        Logger->log<logger::Level::Info>("Frontend identify with the apikey: ", m_frontend.apikey);
        m_frontend.socket->send(reply.data(), reply.size());

        setupBackend(identify);
    } else {
        // send reject message
    }
}

void Session::setupBackend(const ipc::msg::IdentifyFront &_identify)
{
    m_backend.cmd = shell::Builder()
        .newShellCommand(Configuration<config::Global>::Get().Config.Back.Executable)
        .setEnvironement("FIX42-apikey", _identify.apiKey)
        .setEnvironement("FIX42-initiator-address", Configuration<config::Global>::Get().Config.Back.Address + "." + m_session_id)
        .addArgument("--client-ip=127.0.0.1") // todo get ip socket
        .addArgument("--client-port=" + std::to_string(_identify.backport))
        .result();

    Logger->log<logger::Level::Verbose>("New backend setup: ", *(m_backend.cmd));
}