#pragma once

#include "Server/ProcessUnit/Market/Cancel.hpp"
#include "Server/ProcessUnit/Market/DataRequest.hpp"
#include "Server/ProcessUnit/Market/NewOrder.hpp"
#include "Server/ProcessUnit/Market/OBEvent.hpp"
#include "Server/MDSubscribeRegistery.hpp"

#include "Shared/ProcessUnit/ProcessUnit.hpp"
#include "Shared/ProcessUnit/AProcessUnit.hpp"

namespace pu
{
    class MarketContainer : public AProcessUnitBase
    {
        public:
            using MarketTupleQueue = std::tuple<
                QueueMutex<ExecId> &,
                MessageQueue<fix42::msg::NewOrderSingle> &,
                MessageQueue<fix42::msg::OrderCancelRequest> &
            >;

            using MarketDataTupleQueue = std::tuple<
                MDSubscribeRegistery &,
                ts::Queue<Context<data::MDSnapshotRequest>> &
            >;

            MarketContainer(const std::string &_symbol, StringOutputQueue &_tcp_output);
            virtual ~MarketContainer() = default;

            [[nodiscard]] const std::string &getMarketSymbol() const;

            [[nodiscard]] MarketTupleQueue getMarketInput();
            [[nodiscard]] MarketDataTupleQueue getMarketDataInput();

        protected:
            void runtime(std::stop_token _st) final;

        private:
            const std::string m_symbol;

            MDSubscribeRegistery m_registery;

            ProcessUnit<pu::market::OBEvent> m_event;

            OrderBook m_ob;

            QueueMutex<ExecId> m_mutex;

            ProcessUnit<pu::market::DataRequest> m_market_data;
            ProcessUnit<pu::market::NewOrder> m_market_neworder;
            ProcessUnit<pu::market::Cancel> m_market_cancel;
    };
}