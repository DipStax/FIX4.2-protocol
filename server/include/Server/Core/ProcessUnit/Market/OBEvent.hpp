#pragma once

#include "Server/Core/ProcessUnit/data/Market.hpp"
#include "Server/Core/OrderBook.hpp"

#include "Common/Container/AInputProcess.hpp"
#include "Common/Thread/Pool.hpp"

#if !defined(TS_SIZE_OBEVENT) || TS_SIZE_OBEVENT <= 0
    #define TS_SIZE_OBEVENT 1
#endif


namespace pu::market
{
    class OBEvent : public AInputProcess<obs::Event>
    {
        public:
            OBEvent(const std::string &_symbol, InputNetworkOutput &_tcp);
            virtual ~OBEvent() = default;

        protected:
            void onInput(InputType _input) final;

        private:
            bool createEvent(const InputType &_input);

            const std::string m_symbol;

            InputNetworkOutput &m_tcp_output;

            ThreadPool<TS_SIZE_OBEVENT> m_tp;
    };
}