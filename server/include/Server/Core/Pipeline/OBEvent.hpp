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
    class OBEvent : public IProcessUnit
    {
        public:
            OBEvent(const std::string &_name, OrderBook::EventQueue &_input, InUDP &_udp, InOutNetwork &_tcp);
            virtual ~OBEvent() = default;

        protected:
            void runtime(std::stop_token _st);

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