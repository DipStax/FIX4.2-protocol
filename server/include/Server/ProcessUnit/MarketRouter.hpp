#pragma once

#include "Server/ProcessUnit/data/Global.hpp"

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Shared/Message-v2/Message.hpp"

namespace pu
{
    class MarketRouter : public AInputProcess<Context<data::UnparsedMessage>>
    {
        public:
            using MarketTupleQueue = std::tuple<MessageQueue<fix42::msg::NewOrderSingle> &, MessageQueue<fix42::msg::OrderCancelRequest> &>;

            MarketRouter(StringOutputQueue &_error);
            virtual ~MarketRouter() = default;

            void registerMarket(const std::string &_symbol, MarketTupleQueue _queue);

        protected:
            void onInput(InputType _input) final;

        private:
            void processNewOrderSingle(const InputType &_input);
            void processOrderCancelRequest(const InputType &_input);

            template<class T>
            void redirectToMarket(const T &_msg, const InputType &_input);

            StringOutputQueue &m_error;

            std::unordered_map<std::string, MarketTupleQueue> m_markets;
    };
}

#include "Server/ProcessUnit/MarketRouter.inl"