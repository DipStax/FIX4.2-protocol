#pragma once

#include "Server/Core/Pipeline/Naming.hpp"
#include "Server/Core/MarketContainer.hpp"
#include "Server/Core/Pipeline/IProcessUnit.hpp"
#include "Server/Core/Pipeline/ProcessUnit.hpp"

namespace pip
{
    class DataRefresh : public IProcessUnit<Context<MarketInput>>
    {
        public:
            DataRefresh(OrderBook &_ob, InOutNetwork &_output);
            virtual ~DataRefresh() = default;

        protected:
            void runtime(std::stop_token _st);

        private:
            void process(Context<MarketInput> &_input);

            OrderBook &m_ob;

            InMarket m_input;
            InOutNetwork &m_tcp_output;
    };
}