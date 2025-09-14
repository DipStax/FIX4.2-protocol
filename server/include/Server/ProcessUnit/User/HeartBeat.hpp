#pragma once

#include <thread>
#include <optional>

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Shared/ProcessUnit/IProcessUnitStopable.hpp"
#include "Server/ProcessUnit/data/Global.hpp"

#include "Shared/Thread/Pool.hpp"
#include "Shared/Log/ILogger.hpp"

#ifndef PU_HEARTBEAT_TP_SIZE
    #define PU_HEARTBEAT_TP_SIZE 1
#endif

namespace pu::user
{
    /// @brief Process Unit dedicated to processing `HeartBeat` and `TestRequest` message
    class HeartBeatHandler : public AInputProcess<Context<data::UnparsedMessage>>, public IProcessUnitStopable
    {
        public:
            /// @brief Default constructor for the `HeartBeat`/`TestRequest` messages handler
            /// @param _tcp_output Message queue to the TCP/IP output
            HeartBeatHandler(StringOutputQueue &_tcp_output);
            virtual ~HeartBeatHandler() = default;

        protected:
            /// @brief Process in a thread pool every `HeartBeat` and `TestRequest` messages
            /// @param _input Input containing the `HeartBeat` or `TestRequest` message
            void onInput(InputType _input) final;

            /// @brief Setup the thread for `TestRequest` sending
            void setup() final;
            /// @brief Stop the thread for `TestRequest` sending
            void onStop() final;

        private:
            /// @brief Process the `HeartBeat` message
            /// @param _input Input containing the `HeartBeat` message
            void processHeartBeat(const InputType &_input);
            /// @brief Process the `TestRequest` message
            /// @param _input Input containing the `TestRequest` message
            void processTestRequest(const InputType &_input);

            /// @brief Threaded function to send `TestRequest` message when needed
            /// @param _st Thread stop token
            void handle(std::stop_token _st);

            std::jthread m_thread;                  ///< Thread used to handle `TestRequest` message sending

            StringOutputQueue &m_tcp_output;        ///< TCP/IP ouput queue

            ThreadPool<PU_HEARTBEAT_TP_SIZE> m_tp;  ///< Thread pool that process all the incoming message
    };
}