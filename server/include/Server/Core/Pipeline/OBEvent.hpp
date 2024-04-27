#pragma once

#include "Common/Thread/Pool.hpp"
#include "Server/Core/Pipeline/Core.hpp"
#include "Server/Core/Pipeline/Naming.hpp"
#include "Server/Core/OrderBook.hpp"

#ifndef TS_SIZE_OE
    #define TS_SIZE_OE 1
#endif


namespace pip
{
    class OBEvent : public Pipeline<OBEvent>
    {
        public:
            /// @brief Core pipeline type
            using PipeType = Pipeline<OBEvent>;

            OBEvent(const std::string &_name, OrderBook::EventQueue &_input, InUDP &_udp, InOutNetwork &_tcp);
            ~OBEvent();

            /// @brief Run the pipeline
            /// @return Return true if the pipeline as correctly started else false.
            [[nodiscard]] bool start();

            /// @brief Core function of the pipeline determining it's behavior
            void loop();

        protected:
            bool createTcp(const OrderBook::Event &_input);
            bool createUdp(const OrderBook::Event &_input);

        private:
            const std::string m_name;

            OrderBook::EventQueue &m_input;
            InUDP &m_udp;
            InOutNetwork &m_tcp;

            uint64_t m_id = 0;

            ThreadPool<TS_SIZE_OE> m_tp;
    };
}