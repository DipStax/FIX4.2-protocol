#include "Server/ProcessUnit/Market/Router.hpp"
#include "Shared/Log/Manager.hpp"
#include "Shared/Message/Tag.hpp"

namespace pu::market
{
    Router::Router(std::string _symbol, InputNetworkOutput &_tcp_output, InputOBAction &_ob_action)
        : AInputProcess<InputType>("Server/Market/" + _symbol + "/Router"),
        m_symbol(_symbol), m_tcp_output(_tcp_output), m_ob_action(_ob_action)
    {
    }

    void Router::onInput(InputType _input)
    {
        switch (_input.Message.at("35")[0])
        {
            case fix::NewOrderSingle::cMsgType:
            case fix::OrderCancelRequest::cMsgType:
            case fix::OrderCancelReplaceRequest::cMsgType:
                m_ob_action.push(std::move(_input));
                break;
            // case fix::MarketDataRequest::cMsgType:
            //     break;
            // default: (void)treatUnknown(input);
            //     break;
        }
    }

    // bool Router::treatNewOrderSingle(InputType &&_input)
    // {
    //     Logger->log<logger::Level::Info>("(New Order Single) Processing message...");
    //     Context<data::OBActionInput> data(_input.Client, _input.ReceiveTime);
    //     std::pair<bool, fix::Reject> reject = fix::NewOrderSingle::Verify(_input.Message);

    //     if (reject.first) {
    //         Logger->log<logger::Level::Info>("(New Order Single) Request verification failed");
    //         reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
    //         Logger->log<logger::Level::Debug>("(Logout) Reject from (", *(_input.Client), ") moving to TCP output");
    //         m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
    //         return false;
    //     }

    //     data.action = data::OBActionInput::Action::Add;
    //     data.type = (_input.Message.at(fix::Tag::Side) == "3") ? OrderType::Bid : OrderType::Ask;
    //     data.price = utils::to<Price>(_input.Message.at(fix::Tag::Price));
    //     data.order.userId = _input.Client->getUserId();
    //     data.order.orderId = _input.Message.at(fix::Tag::ClOrdID);
    //     data.order.quantity = utils::to<Quantity>(_input.Message.at(fix::Tag::OrderQty));
    //     Logger->log<logger::Level::Debug>("(New Order Single) Moving to target OrderBook Container new order: (", data.order.orderId,") from: (", data.order.userId, ")");
    //     m_ob_action.push(std::move(data));
    //     return true;
    // }

    // bool Router::treatOrderCancelRequest(InputType &&_input)
    // {
    //     Logger->log<logger::Level::Info>("(Order Cancel Request) Processing message...");
    //     Context<data::OBActionInput> data(_input.Client, _input.ReceiveTime);
    //     std::pair<bool, fix::Reject> reject = fix::OrderCancelRequest::Verify(_input.Message);

    //     if (reject.first) {
    //         Logger->log<logger::Level::Info>("(Order Cancel Request) Request verification failed");
    //         reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
    //         Logger->log<logger::Level::Debug>("(Order Cancel Request) Reject from (", *(_input.Client), ") moving to TCP output");
    //         m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
    //         return false;
    //     }
    //     data.action = data::OBActionInput::Action::Cancel;
    //     data.order.orderId = _input.Message.at(fix::Tag::OrigClOrdID);
    //     data.order.userId = _input.Client->getUserId();
    //     data.type = (_input.Message.at(fix::Tag::Side) == "3") ? OrderType::Bid : OrderType::Ask;
    //     Logger->log<logger::Level::Debug>("(New Order Single) Moving to target OrderBook Container cancel order: (", data.order.orderId,") from: (", data.order.userId, ")");
    //     m_ob_action.push(std::move(data));
    //     return true;
    // }

    // bool Router::treatOrderCancelReplaceRequest(InputType &&_input)
    // {
    //     Logger->log<logger::Level::Info>("(Order Cancel Replace) Processing message...");
    //     Context<data::OBActionInput> data(_input.Client, _input.ReceiveTime);
    //     std::pair<bool, fix::Reject> reject = fix::OrderCancelReplaceRequest::Verify(_input.Message);

    //     if (reject.first) {
    //         Logger->log<logger::Level::Info>("(Order Cancel Replace) Request verification failed");
    //         reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
    //         Logger->log<logger::Level::Debug>("(Order Cancel Replace) Reject from (", *(_input.Client), ") moving to TCP output");
    //         m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
    //         return false;
    //     }
    //     data.action = data::OBActionInput::Action::Modify;
    //     data.order.userId = _input.Client->getUserId();
    //     data.target = _input.Message.at(fix::Tag::OrigClOrdID);
    //     data.order.orderId = _input.Message.at(fix::Tag::ClOrdID);
    //     data.order.quantity = utils::to<Quantity>(_input.Message.at(fix::Tag::OrderQty));
    //     data.price = utils::to<Price>(_input.Message.at(fix::Tag::Price));
    //     data.type = (_input.Message.at(fix::Tag::Side) == "3") ? OrderType::Bid : OrderType::Ask;
    //     Logger->log<logger::Level::Debug>("(New Order Replace) Moving to target OrderBook Container replace order: (", data.order.orderId,") from: (", data.order.userId, ")");
    //     m_ob_action.push(std::move(data));
    //     return true;
    // }
}