#pragma once

#include <QObject>

#include "Client/Shared/IPC/Message/Authentification.hpp"
#include "Client/Shared/IPC/Message/TokenValidation.hpp"
#include "Client/Shared/IPC/IPCNetworkManager.hpp"

#include "Shared/Log/ILogger.hpp"
#include "Shared/Network/Buffer.hpp"
#include "Shared/Network/Socket.hpp"

class InitiatorManager : public QObject, IPCNetworkManager<net::INetTcp>
{
    Q_OBJECT

    public:
        static InitiatorManager *Instance();

    public slots:
        void startConnection();
        void send(const net::Buffer &_buffer);

    signals:
        void connectionReady();

        void received_IdentifyFront(ipc::msg::AuthInitiatorToFront _auth);
        void received_ValidationToken(ipc::msg::InitiatorToFrontValidToken _token);

    public:
        void stop();

    protected:
        InitiatorManager(QObject *_parent = nullptr);

        void onError(int _errno) final;
        void onDisconnection() final;
        void onWrongSize(const std::vector<std::byte> &_byte, int _readsize) final;
        void onWrongBodySize(const std::vector<std::byte> &_byte, int _readsize) final;

        void onReceive(net::Buffer &_buffer) final;

    private:
        std::stop_source m_stopsource{};

        inline static InitiatorManager *m_instance = nullptr;
};