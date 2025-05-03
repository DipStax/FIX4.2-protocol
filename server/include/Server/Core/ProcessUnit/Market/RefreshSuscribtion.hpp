#pragma once

#include "Server/Core/ProcessUnit/interface/IProcessUnit.hpp"
#include "Server/Core/ProcessUnit/Naming.hpp"

namespace pu::market
{
    class RefreshSuscribtion : public IProcessUnit<Context<MarketInput>>
    {
        public:
            RefreshSuscribtion(OrderBook &_ob, InOutNetwork &_output);
            virtual ~RefreshSuscribtion() = default;

            [[nodiscard]] QueueInputType &getInput();

        protected:
            std::string getThreadName() const;

            void runtime(std::stop_token _st);

        private:
            void process(Context<MarketInput> &_input);

            OrderBook &m_ob;

            QueueInputType m_input;
            InOutNetwork &m_tcp_output;
    };
}