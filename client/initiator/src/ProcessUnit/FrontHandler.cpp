#include "Client/Initiator/Configuration.hpp"
#include "Client/Initiator/Config.hpp"
#include "Client/Initiator/SessionManager.hpp"
#include "Client/Initiator/ProcessUnit/FrontHandler.hpp"

#include "Client/Shared/IPC/Header.hpp"

namespace pu
{
    FrontHandler::FrontHandler()
        : AProcessUnitBase("Initiator/Session/FrontHandler")
    {
        m_acceptor.listen(Configuration<config::Global>::Get().Config.Front.Port);
        m_acceptor.setBlocking(false);
    }

    void FrontHandler::runtime(std::stop_token _st)
    {
        std::shared_ptr<net::INetTcp> client;
        std::vector<std::shared_ptr<net::INetTcp>> clients;

        while (_st.stop_requested()) {
            client = m_acceptor.accept();
            if (client != nullptr) {
                Logger->log<logger::Level::Info>("New client connected");
                m_selector.client(client);
                SessionManager::Instance().newSession(client);
            }
            clients = m_selector.pull();
            if (clients.size() != 0)
                Logger->log<logger::Level::Info>("Event pulled from: ", clients.size(), " client(s)");
            for (const std::shared_ptr<net::INetTcp> &_client : clients) {
                std::shared_ptr<Session> session = SessionManager::Instance().findSession(_client);

                if (process(_client, session)) {
                    // disconnect client
                }
            }
        }
    }

    bool FrontHandler::process(const std::shared_ptr<net::INetTcp> &_socket, std::shared_ptr<Session> &_session)
    {
        ipc::Header header;
        int error = 0;
        std::vector<std::byte> bytes = _socket->receive(sizeof(ipc::Header), error);
        net::Buffer buffer;

        if (error != sizeof(ipc::Header)) {
            if (error == -1)
                Logger->log<logger::Level::Error>("Error when receivin data from back: ", strerror(errno));
            else if (error == 0)
                Logger->log<logger::Level::Fatal>("Frontend client disconnected");
            else
                Logger->log<logger::Level::Warning>("Unable to read ipc::Header from socket, read size: ", error, " != ", sizeof(ipc::Header));
            return false;
        }

        buffer.append(bytes.data(), bytes.size());
        buffer >> header;
        m_tp.enqueue([_session, _header = std::move(header), _buffer = std::move(buffer)] () mutable {
            _session->received(_header, _buffer, Session::Side::Front);
        });
        return true;
    }
}