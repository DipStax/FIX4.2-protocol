#pragma once

#include <QObject>
#include <QThread>
#include <memory>

#include "Shared/ProcessUnit/ProcessUnit.hpp"
#include "Shared/Network/Socket.hpp"
#include "Shared/Network/Buffer.hpp"
#include "Shared/Log/ILogger.hpp"

#include "Client/Common/IPC/Message/Message.hpp"

class BackManager : public QObject
{
    Q_OBJECT

    public:
        static BackManager *Instance();

    public slots:
        void startConnection();
        void send(const net::Buffer &_buffer);

    signals:
        void received_Status(PUStatus _status);
        void received_Logon(ipc::msg::Logon _exec);
        void received_ExecutionNew(ipc::msg::Execution _exec);
        void received_ExecutionEvent(ipc::msg::Execution _exec);

    protected:
        BackManager(QObject *_parent = nullptr);

    private:
        void ipcReceived(net::Buffer &_buffer);

        std::shared_ptr<net::UnixStream> m_socket = nullptr;

        std::unique_ptr<logger::ILogger> Logger = nullptr;

        inline static BackManager *m_instance = nullptr;
};