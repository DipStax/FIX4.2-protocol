#pragma once

#include "Server/ProcessUnit/data/Global.hpp"
#include "Server/ProcessUnit/data/DataRequest.hpp"
#include "Server/MDSubscribeRegistery.hpp"

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Shared/Message-v2/Message.hpp"
#include "Shared/Thread/Queue.hpp"


namespace pu
{
    class MarketDataRouter : public AInputProcess<Context<data::UnparsedMessage>>
    {
        public:
            using MarketDataTupleQueue = std::tuple<
                MDSubscribeRegistery &,
                ts::Queue<Context<data::MDSnapshotRequest>> &
            >;

            MarketDataRouter(StringOutputQueue &_tcp_output);
            virtual ~MarketDataRouter() = default;

            void registerMarket(const std::string &_symbol, MarketDataTupleQueue &_queue);

        protected:
            void onInput(InputType _input) final;

        private:
            void handleSnapshot(const InputType &_input, const fix42::msg::MarketDataRequest &_mdreq);
            void handleSubscribe(const InputType &_input, const fix42::msg::MarketDataRequest &_mdreq);

            StringOutputQueue &m_tcp_output;

            std::unordered_map<std::string, MarketDataTupleQueue> m_market;
    };
}