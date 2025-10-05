#pragma once

#include <memory>

#include <QObject>

#include "Client/Shared/IPC/Message/Message.hpp"

#include "Shared/Log/ILogger.hpp"
#include "Shared/Network/Buffer.hpp"
#include "Shared/Network/Socket.hpp"
#include "Shared/ProcessUnit/ProcessUnit.hpp"

class BackManager : public QObject
{
    Q_OBJECT

    public:
        static BackManager *Instance();

        void setTargetPort(uint32_t _port);

    public slots:
        void startConnection();
        void send(const net::Buffer &_buffer);
        void stop();

    signals:
        void received_TokenValidation(ipc::msg::BackToFrontValidToken _token);
        void received_Status(PUStatus _status);
        void received_Logon(ipc::msg::Logon _exec);
        void received_ExecutionNew(ipc::msg::ExecutionNew _exec);
        void received_ExecutionEvent(ipc::msg::ExecutionEvent _exec);

    protected:
        BackManager(QObject *_parent = nullptr);

    private:
        void ipcReceived(net::Buffer &_buffer);

        void emit_TokenValidation(net::Buffer &_buffer);
        void emit_Status(net::Buffer &_buffer);
        void emit_Logon(net::Buffer &_buffer);
        void emit_ExecutionNew(net::Buffer &_buffer);
        void emit_ExecutionEvent(net::Buffer &_buffer);

        bool m_running = false;

        uint32_t m_port = 0;

        std::shared_ptr<net::INetTcp> m_socket = nullptr;

        std::unique_ptr<logger::ILogger> Logger = nullptr;

        inline static BackManager *m_instance = nullptr;
};