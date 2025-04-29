#pragma once

#include "Server/Core/Pipeline/Naming.hpp"
#include "Server/Core/MarketContainer.hpp"
#include "Server/Core/Pipeline/IProcessUnit.hpp"
#include "Server/Core/Pipeline/ProcessUnit.hpp"

namespace pip
{
    class DataRefresh : public IProcessUnit
    {
        public:
            DataRefresh(std::map<std::string, ProcessUnit<MarketContainer>> &_markets, InMarketData &_input, InOutNetwork &_output);
            virtual ~DataRefresh() = default;

        protected:
            void runtime(std::stop_token _st);

        private:
            void process(MarketDataInput &_input);

            std::map<std::string, ProcessUnit<MarketContainer>> &m_markets;

            InMarketData &m_input;
            InOutNetwork &m_output;
    };
}