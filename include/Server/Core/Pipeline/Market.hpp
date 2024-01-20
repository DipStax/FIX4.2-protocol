#pragma once

#include "Common/Thread/Pool.hpp"
#include "Server/Core/Pipeline/Core.hpp"
#include "Server/Core/Pipeline/Naming.hpp"

#ifndef TS_SIZE_OB
    #define TS_SIZE_OB 1
#endif

namespace pip
{
    /// @brief Pipeline managing the OrderBook.
    class Market : public Pipeline<Market>
    {
        public:
            /// @brief Construct the pipeline.
            /// @param _ob OrderBook reference.
            /// @param _input Input data queue.
            /// @param _output Output data queue.
            Market(const std::string &_name, OrderBook &_ob, ActionToMarket &_input, MarketToNet &_output);
            /// @brief Stop the pipeline and the destroy it.
            ~Market();

            /// @brief Run the pipeline
            /// @return Return true if the pipeline as correctly started else false.
            [[nodiscard]] bool start();

            /// @brief Core function of the pipeline determining it's behavior.
            void loop();

        private:
            /// @brief Apply the action received by the pip::Action pipeline on the OrderBook.
            /// @param _data Data to build and run action on the OrderBook.
            void process(MarketIn &_data);

            bool runAdd(MarketIn _data);
            bool runModify(MarketIn _data);
            bool runCancel(MarketIn _data);

            const std::string m_name;

            ActionToMarket &m_input;        ///< Intput data queue.
            MarketToNet &m_output;          ///< Output data queue.

            ThreadPool<TS_SIZE_OB> m_tp;    ///< Thread pool to run async processing.

            OrderBook &m_ob;                ///< OrderBook manage by the pipeline.
    };
}