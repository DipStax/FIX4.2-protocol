#pragma once

#include "Server/Core/Pipeline/Core.hpp"
#include "Server/Core/Pipeline/Naming.hpp"
#include "Server/Core/ClientSocket.hpp"
#include "Server/Core/MarketContainer.hpp"

namespace pip
{
    class DataRefresh : public Pipeline<DataRefresh>
    {
        public:
            /// @brief Core pipeline type
            using PipeType = Pipeline<DataRefresh>;

            DataRefresh(std::map<std::string, MarketContainer> &_markets, InMarketData &_input, InOutNetwork &_output);
            ~DataRefresh();

            /// @brief Run the pipeline
            /// @return Return true if the pipeline as correctly started else false.
            [[nodiscard]] bool start();

            /// @brief Core function of the pipeline determining it's behavior
            void loop();

        private:
            void process(MarketDataInput &_input);

            std::map<std::string, MarketContainer> &m_markets;

            InMarketData &m_input;
            InOutNetwork &m_output;
    };
}