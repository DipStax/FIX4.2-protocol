#include "Common/Core/Logger.hpp"
#include "Client/Processor/ReportHandler.hpp"
#include "Common/Message/Tag.hpp"

namespace proc
{
    bool ReportHandler::handle(fix::Serializer::AnonMessage &_msg, const Context &_ctx) const
    {
        return _msg.at(fix::Tag::MsgType) == fix::Reject::MsgType || _msg.at(fix::Tag::MsgType) == fix::ExecutionReport::MsgType;
    }

    std::optional<fix::Message> ReportHandler::process(fix::Serializer::AnonMessage &_msg, Context &_ctx)
    {
        if (_msg.at(fix::Tag::MsgType) == fix::Reject::MsgType)
            Logger::Log("[ReportHandler] Received reject message: { RefSeqNum: '", _msg.at(fix::Tag::RefSeqNum), "', Text: '", _msg.at(fix::Tag::Text), "' }");
        else if (_msg.at(fix::Tag::MsgType) == fix::ExecutionReport::MsgType)
            treatExecutionReport(_msg, _ctx);
        return {};
    }

    std::optional<fix::Message> ReportHandler::build(char _tag, const Context &_ctx) const
    {
        std::ignore = _tag;
        std::ignore = _ctx;

        return {};
    }

    void ReportHandler::treatExecutionReport(fix::Serializer::AnonMessage &_msg, Context &_ctx)
    {
        Logger::Log("[ReportHandler] Received a new Execution Report");
        OrderClient order;

        order.status = static_cast<OrderStatus>(utils::to<int>(_msg.at(fix::Tag::OrdStatus)));
        if (order.status == OrderStatus::Rejected) {
            Logger::Log("[ReportHandler] Order rejected: [", _msg.at(fix::Tag::OrderID), "] reason: '", _msg.at(fix::Tag::Text), "'");
            return;
        }
        order.orderId = _msg.at(fix::Tag::OrderID);
        order.quantity = utils::to<Quantity>( _msg.at(fix::Tag::OrderQty));
        order.price = utils::to<double>(_msg.at(fix::Tag::Price));
        order.type = (_msg.at(fix::Tag::Side) == "1") ? OrderType::Bid : OrderType::Ask;
        order.symbol = _msg.at(fix::Tag::Symbol);
        Logger::Log("[ReportHandler] {NewOrder} New order: ", order);
        if (_ctx.userInfos.getOrderToCancel() != "" && order.status == OrderStatus::Replaced) {
            _ctx.userInfos.replaceOrder(_ctx.userInfos.getOrderToCancel(), order);
            _ctx.userInfos.clearOrderToCancel();
        }
        if (order.status == OrderStatus::Filled || order.status == OrderStatus::Canceld)
            _ctx.userInfos.removeOrder(order.orderId);
        else
            _ctx.userInfos.addOrder(order);
        _ctx.userInfos.addHistory(order);
    }
}