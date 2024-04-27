#pragma once

#include "Client/Processor/IProcessor.hpp"

namespace proc
{
    class User : public proc::IMessage, public proc::IEntry
    {
        public:
            [[nodiscard]] virtual bool handle(fix::Serializer::AnonMessage &_msg, const Context &_ctx) const override final;
            [[nodiscard]] virtual bool handle(const Entry &_entry, const Context &_ctx) const override final;

            [[nodiscard]] virtual std::optional<fix::Message> process(fix::Serializer::AnonMessage &_msg, Context &_ctx) override final;
            [[nodiscard]] virtual std::optional<fix::Message> process(const Entry &_entry, Context &_ctx) override final;

            [[nodiscard]] virtual std::optional<fix::Message> build(char _tag, const Context &_ctx) const override final;

        protected:
            [[nodiscard]] fix::Message buildLogon(const UserId &_id) const;
            [[nodiscard]] fix::Message buildLogout() const;

            [[nodiscard]] fix::Message buildNewOrder(const std::string &_orderId, const std::string &_quantity, const std::string &_price, const std::string &_side, const std::string &_symbol) const;
            [[nodiscard]] fix::Message buildOrderCancel(const std::string &_cancelOrderId, const std::string &_targetOrderId, const std::string &_symbol, const std::string &_side) const;
            [[nodiscard]] fix::Message buildOrderCancelReplace(const std::string &_cancelReplaceOrderId, const std::string &_targetOrderId, const std::string &_quantity, const std::string &_price, const std::string &_side, const std::string &_symbol) const;
    };
}