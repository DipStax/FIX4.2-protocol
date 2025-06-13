#pragma once

#include <thread>

#include "Common/Container/ProcessUnit.hpp"
#include "Common/Log/ILogger.hpp"
#include "Common/Network/Acceptor.hpp"
#include "Common/Network/Buffer.hpp"
#include "Common/Network/Socket.hpp"
#include "Common/Thread/Queue.hpp"

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