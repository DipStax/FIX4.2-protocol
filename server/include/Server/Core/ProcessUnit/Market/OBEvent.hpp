#pragma once

#include "Server/Core/ProcessUnit/interface/IProcessUnit.hpp"
#include "Server/Core/ProcessUnit/data/Market.hpp"
#include "Common/Thread/Pool.hpp"
#include "Server/Core/OrderBook.hpp"

#ifndef TS_SIZE_OE
    #define TS_SIZE_OE 1
#endif


namespace pu::market
{
    class OBEvent : public IProcessUnit<data::OBEventInput>
    {
        public:
            OBEvent(const std::string &_symbol, InputUdp &_udp, InputNetworkOutput &_tcp);
            virtual ~OBEvent() = default;

            [[nodiscard]] QueueInputType &getInput();

        protected:
            std::string getThreadName() const;

            void runtime(std::stop_token _st);

        protected:
            bool createTcp(const OrderBook::Event &_input);
            bool createUdp(const OrderBook::Event &_input);

        private:
            const std::string m_symbol;

            QueueInputType m_input;

            InputUdp &m_udp_output;
            InputNetworkOutput &m_tcp_output;

            ThreadPool<TS_SIZE_OE> m_tp;
    };
}