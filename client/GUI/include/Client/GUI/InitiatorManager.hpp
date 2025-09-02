#pragma once

#include <QObject>

#include "Client/Shared/IPC/Message/Identify.hpp"

#include "Shared/Log/ILogger.hpp"
#include "Shared/Network/Buffer.hpp"
#include "Shared/Network/Socket.hpp"

class InitiatorManager : public QObject
{
    Q_OBJECT

    public:
        static InitiatorManager *Instance();

    public slots:
        void startConnection();
        void send(const net::Buffer &_buffer);

    signals:
        void connectionReady();
        void received_IdentifyFront(ipc::msg::IdentifyFront _identify);

    protected:
        InitiatorManager(QObject *_parent = nullptr);

    private:
        void ipcReceived(net::Buffer &_buffer);

        std::shared_ptr<net::INetTcp> m_socket = nullptr;

        std::unique_ptr<logger::ILogger> Logger = nullptr;

        inline static InitiatorManager *m_instance = nullptr;
};