#pragma once

#include "Common/Container/IProcessUnit.hpp"
#include "Server/Core/ProcessUnit/data/Market.hpp"

namespace pu::market
{
    class RefreshSuscribtion : public IProcessUnit<Context<data::RefreshSubInput>>
    {
        public:
            RefreshSuscribtion(OrderBook &_ob, InputNetworkOutput &_output);
            virtual ~RefreshSuscribtion() = default;

            [[nodiscard]] QueueInputType &getInput();

        protected:
            void runtime(std::stop_token _st);

        private:
            void process(InputType &_input);

            OrderBook &m_ob;

            QueueInputType m_input;
            InputNetworkOutput &m_tcp_output;
    };
}