#pragma once

#include <thread>

#include "Shared/ProcessUnit/ProcessUnit.hpp"
#include "Shared/Log/ILogger.hpp"
#include "Shared/Network/Acceptor.hpp"
#include "Shared/Network/Buffer.hpp"
#include "Shared/Network/Socket.hpp"
#include "Shared/Thread/Queue.hpp"

class FrontManager
{
    public:
        ~FrontManager() = default;

        static FrontManager &Instance();

        void notify(const net::Buffer &_buffer);

        bool wait_frontend();

        ts::Queue<net::Buffer> &getMessageQueue();

    private:
        FrontManager();

        void receiveLoop(std::stop_token _st);

        ts::Queue<net::Buffer> m_output;

        std::jthread m_thread;

        net::Acceptor<net::UnixStream> m_acceptor;
        net::Acceptor<net::UnixStream>::Client m_socket;

        std::unique_ptr<logger::ILogger> Logger = nullptr;
};