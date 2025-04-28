#pragma once

#include <chrono>

#include "Server/Core/Pipeline/Naming.hpp"
#include "Server/Core/Pipeline/IProcessUnit.hpp"

#ifndef UDP_TICK
    #define UDP_TICK 1
#endif

#ifndef UDP_CLEAR
    #define UDP_CLEAR 2
#endif

#ifndef UDP_MAX_MSG
    #define UDP_MAX_MSG 20
#endif

namespace pip
{
    class UDPOutNetwork : public IProcessUnit
    {
        public:
            /// @brief Construct the pipeline.
            /// @param _input Input data queue.
            UDPOutNetwork(InUDP &_input, uint32_t _port);
            virtual ~UDPOutNetwork() = default;

        protected:
            void runtime(std::stop_token _st);

        protected:
            /// @brief Clean the message vector if a message as expire.
            void clean();

        private:
            std::vector<std::pair<std::chrono::steady_clock::time_point, data::UDPPackage>> m_message;

            net::udp::Socket m_socket;

            InUDP &m_input;
    };
}