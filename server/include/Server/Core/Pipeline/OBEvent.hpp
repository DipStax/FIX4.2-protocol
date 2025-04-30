#pragma once

#include "Common/Thread/Pool.hpp"
#include "Server/Core/Pipeline/Naming.hpp"
#include "Server/Core/OrderBook.hpp"
#include "Server/Core/Pipeline/IProcessUnit.hpp"

#ifndef TS_SIZE_OE
    #define TS_SIZE_OE 1
#endif


namespace pip
{
    class OBEvent : public IProcessUnit<OrderBook::Event>
    {
        public:
            OBEvent(const std::string &_name, InUDP &_udp, InOutNetwork &_tcp);
            virtual ~OBEvent() = default;

            [[nodiscard]] InputType &getInput();

        protected:
            void runtime(std::stop_token _st);

        protected:
            bool createTcp(const OrderBook::Event &_input);
            bool createUdp(const OrderBook::Event &_input);

        private:
            const std::string m_name;

            OrderBook::EventQueue m_input;

            InUDP &m_udp_output;
            InOutNetwork &m_tcp_output;

            ThreadPool<TS_SIZE_OE> m_tp;
    };
}