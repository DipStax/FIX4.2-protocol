#include "Client/Initiator/ProcessUnit/BackHandler.hpp"

#include "Client/Initiator/SessionManager.hpp"

namespace pu
{
    BackHandler::BackHandler()
        : AProcessUnitBase("Initiator/BackHandler")
    {
        SessionManager::OnRemoveSession([this] (const std::shared_ptr<Session> &_session) {
            Logger->log<logger::Level::Info>("Removin session from selector: ", _session->getId());
            m_selector.erase(_session->getBackSocket());
        });
        m_acceptor.listen(Configuration<config::Global>::Get().Config.Back.Address);
        Logger->log<logger::Level::Debug>("Listening on socket: ", Configuration<config::Global>::Get().Config.Back.Address);
        m_acceptor.setBlocking(false);
    }

    void BackHandler::runtime(std::stop_token _st)
    {
        std::shared_ptr<net::UnixStream> client;
        std::vector<std::shared_ptr<net::UnixStream>> clients;

        while (!_st.stop_requested()) {
            client = m_acceptor.accept();
            if (client != nullptr) {
                Logger->log<logger::Level::Info>("New backend connected");
                m_selector.client(client);
            }
            clients = m_selector.pull();
            if (clients.size() != 0)
                Logger->log<logger::Level::Info>("Event pulled from: ", clients.size(), " backend(s)");
            for (const std::shared_ptr<net::UnixStream> &_client : clients) {
                if (process(_client)) {
                    // todo do something about it
                }
            }
        }
    }

    bool BackHandler::process(const std::shared_ptr<net::UnixStream> &_socket)
    {
        ipc::Header header;
        int error = 0;
        std::vector<std::byte> bytes = _socket->receive(sizeof(ipc::Header), error);
        net::Buffer buffer;
        std::shared_ptr<Session> session = nullptr;

        if (error != sizeof(ipc::Header)) {
            if (error == -1)
                Logger->log<logger::Level::Error>("Error when receiving data from back: ", strerror(errno));
            else if (error == 0)
                Logger->log<logger::Level::Fatal>("Frontend client disconnected");
            else
                Logger->log<logger::Level::Warning>("Unable to read ipc::Header from socket, read size: ", error, " != ", sizeof(ipc::Header));
            return false;
        }

        buffer.append(bytes.data(), bytes.size());
        buffer >> header;

        bytes = _socket->receive(header.BodySize, error);
        buffer.clear();
        buffer.append(bytes.data(), bytes.size());

        if (header.MsgType == ipc::MessageType::BackToInitiatorAuth) {
            Logger->log<logger::Level::Info>("Received auth request from backend");
            ipc::msg::AuthBackToInitiator auth;

            buffer >> auth;
            Logger->log<logger::Level::Debug>("Looking for info on session with: ", auth);
            session = SessionManager::Instance().findSession(auth.apikey);
            session->setBackendSocket(_socket);
            buffer.reset();
        } else {
            session = SessionManager::Instance().findSession(_socket);
        }
        m_tp.enqueue([session, _header = std::move(header), _buffer = std::move(buffer)] () mutable {
            session->received(_header, _buffer, Session::Side::Back);
        });
        return true;
    }
}