#pragma once

#include <memory>
#include <vector>

#include "Client/Handler/UDPHandler.hpp"
#include "Client/Handler/UserInput.hpp"
#include "Client/Handler/TCPHandler.hpp"
#include "Client/Processor/IProcessor.hpp"
#include "Common/Network/Ip.hpp"
#include "Common/Thread/Queue.hpp"

class Core
{
    public:
        Core(const net::Ip &_ip, uint32_t _tcp, uint32_t _udp);
        ~Core();

        void start();

        void stop();

    private:
        std::vector<std::shared_ptr<proc::IMessage>> m_proc_tcp;
        std::vector<std::shared_ptr<proc::IUDP>> m_proc_udp;
        std::vector<std::shared_ptr<proc::IEntry>> m_proc_entry;

        void setContext(fix::Message &_msg);

        bool m_running = false;

        Context m_context;

        io::UserInput m_input;
        io::TCPHandler m_tcp;
        io::UDPHandler m_udp;

        std::thread m_thread;
};