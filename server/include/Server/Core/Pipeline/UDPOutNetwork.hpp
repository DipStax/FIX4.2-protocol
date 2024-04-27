#pragma once

#include <chrono>

#include "Server/Core/Pipeline/Core.hpp"
#include "Server/Core/Pipeline/Naming.hpp"

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
    class UDPOutNetwork : public Pipeline<UDPOutNetwork>
    {
        public:
            /// @brief Core pipeline type.
            using PipeType = Pipeline<UDPOutNetwork>;

            /// @brief Construct the pipeline.
            /// @param _input Input data queue.
            UDPOutNetwork(InUDP &_input, uint32_t _port);
            ~UDPOutNetwork();

            /// @brief Run the pipeline
            /// @return Return true if the pipeline as correctly started else false.
            [[nodiscard]] bool start();

            /// @brief Core function of the pipeline determining it's behavior
            void loop();

        protected:
            /// @brief Clean the message vector if a message as expire.
            void clean();

        private:
            std::vector<std::pair<std::chrono::steady_clock::time_point, data::UDPPackage>> m_message;

            net::udp::Socket m_socket;

            InUDP &m_input;
    };
}