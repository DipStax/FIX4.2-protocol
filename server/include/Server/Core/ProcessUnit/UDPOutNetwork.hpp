#pragma once

#include <chrono>

#include "Server/Core/ProcessUnit/interface/IProcessUnit.hpp"
#include "Server/Core/ProcessUnit/data/Global.hpp"

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
    class UDPOutNetwork : public IProcessUnit<data::UDPPackage>
    {
        public:
            /// @brief Construct the pipeline.
            /// @param _input Input data queue.
            UDPOutNetwork(uint32_t _port);
            virtual ~UDPOutNetwork() = default;

            [[nodiscard]] QueueInputType &getInput();

        protected:
            std::string getThreadName() const;

            void runtime(std::stop_token _st);

        protected:
            /// @brief Clean the message vector if a message as expire.
            void clean();

        private:
            std::vector<std::pair<std::chrono::steady_clock::time_point, data::UDPPackage>> m_message;

            net::udp::Socket m_socket;

            QueueInputType m_input;
    };
}