#pragma once

#include <QObject>
#include <QThread>
#include <memory>

#include "Common/Network/Socket.hpp"
#include "Common/Network/Buffer.hpp"
#include "Common/Log/ILogger.hpp"

class BackManager : public QObject
{
    Q_OBJECT

    public:
        static BackManager *Instance();

    public slots:
        void startConnection();
        void send(const net::Buffer &_buffer);

    signals:
        void received(net::Buffer &_buffer);
        void closed();

    protected:
        BackManager(QObject *_parent = nullptr);

    private:
        std::shared_ptr<net::UnixStream> m_socket = nullptr;

        std::unique_ptr<logger::ILogger> Logger = nullptr;

        inline static BackManager *m_instance = nullptr;
};