#pragma once

#include "Server/Core/ProcessUnit/interface/IProcessUnit.hpp"
#include "Server/Core/ProcessUnit/Naming.hpp"
#include "Common/Thread/Pool.hpp"
#include "Server/Core/OrderBook.hpp"

#ifndef TS_SIZE_OE
    #define TS_SIZE_OE 1
#endif


namespace pu::market
{
    class OBEvent : public IProcessUnit<OrderBook::Event>
    {
        public:
            OBEvent(const std::string &_name, InUDP &_udp, InOutNetwork &_tcp);
            virtual ~OBEvent() = default;

            [[nodiscard]] QueueInputType &getInput();

        protected:
            void runtime(std::stop_token _st);

        protected:
            bool createTcp(const OrderBook::Event &_input);
            bool createUdp(const OrderBook::Event &_input);

        private:
            const std::string m_name;

            QueueInputType m_input;

            InUDP &m_udp_output;
            InOutNetwork &m_tcp_output;

            ThreadPool<TS_SIZE_OE> m_tp;
    };
}