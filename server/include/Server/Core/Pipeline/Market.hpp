#pragma once

#include "Common/Thread/Pool.hpp"
#include "Server/Core/Pipeline/Naming.hpp"
#include "Server/Core/Pipeline/IProcessUnit.hpp"

#ifndef TS_SIZE_OB
    #define TS_SIZE_OB 1
#endif

namespace pip
{
    /// @brief Pipeline managing the OrderBook.
    class Market : public IProcessUnit
    {
        public:
            /// @brief Construct the pipeline.
            /// @param _ob OrderBook reference.
            /// @param _input Input data queue.
            /// @param _output Output data queue.
            Market(OrderBook &_ob, InMarket &_input, InOutNetwork &_output);
            virtual ~Market() = default;

        protected:
            void runtime(std::stop_token _st);

        private:
            /// @brief Apply the action received by the pip::Action pipeline on the OrderBook.
            /// @param _data Data to build and run action on the OrderBook.
            void process(Context<MarketInput> &_data);

            bool runAdd(const Context<MarketInput> &_data);
            bool runModify(const Context<MarketInput> &_data);
            bool runCancel(const Context<MarketInput> &_data);

            const std::string m_name;

            InMarket &m_input;        ///< Intput data queue.
            InOutNetwork &m_output;          ///< Output data queue.

            ThreadPool<TS_SIZE_OB> m_tp;    ///< Thread pool to run async processing.

            OrderBook &m_ob;                ///< OrderBook manage by the pipeline.
    };
}