#pragma once

#include "Server/Core/ProcessUnit/interface/IProcessUnit.hpp"
#include "Server/Core/ProcessUnit/Naming.hpp"
#include "Common/Thread/Pool.hpp"

#ifndef TS_SIZE_OB
    #define TS_SIZE_OB 1
#endif

namespace pu::market
{
    /// @brief Pipeline managing the OrderBook.
    class OBAction : public IProcessUnit<Context<MarketInput>>
    {
        public:
            /// @brief Construct the pipeline.
            /// @param _ob OrderBook reference.
            /// @param _input Input data queue.
            /// @param _output Output data queue.
            OBAction(OrderBook &_ob, InOutNetwork &_output);
            virtual ~OBAction() = default;

            [[nodiscard]] QueueInputType &getInput();

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

            QueueInputType m_input;        ///< Intput data queue.
            InOutNetwork &m_tcp_output;          ///< Output data queue.

            ThreadPool<TS_SIZE_OB> m_tp;    ///< Thread pool to run async processing.

            OrderBook &m_ob;                ///< OrderBook manage by the pipeline.
    };
}