#pragma once

#include <chrono>

#include "Common/Container/IProcessUnit.hpp"
#include "Server/Core/ProcessUnit/data/Global.hpp"

#include "Common/Log/ILogger.hpp"

#ifndef UDP_TICK
    #define UDP_TICK 1
#endif

#ifndef UDP_CLEAR
    #define UDP_CLEAR 2
#endif

#ifndef UDP_MAX_MSG
    #define UDP_MAX_MSG 20
#endif

namespace pu
{
    class UdpOutputNetwork : public IProcessUnit<data::UDPPackage>
    {
        public:
            UdpOutputNetwork(uint32_t _port);
            virtual ~UdpOutputNetwork() = default;

            [[nodiscard]] QueueInputType &getInput();

        protected:
            void runtime(std::stop_token _st);

        protected:
            void clean();

        private:
            std::vector<std::pair<std::chrono::steady_clock::time_point, data::UDPPackage>> m_message;

            net::INetUdp m_socket;

            QueueInputType m_input;

            std::unique_ptr<log::ILogger> Logger = nullptr;
    };
}