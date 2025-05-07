#include "Server/Core/ProcessUnit/Market/Router.hpp"

namespace pu::market
{
    Router::Router(std::string _symbol, InputNetworkOutput &_tcp_output, InputOBAction &_ob_action)
        : m_symbol(_symbol), m_tcp_output(_tcp_output), m_ob_action(_ob_action)
    {
    }

    Router::QueueInputType &Router::getInput()
    {
        return m_input;
    }

    void Router::runtime(std::stop_token _st)
    {
        InputType input;

        while (!_st.stop_requested()) {
            while (!m_input.empty()) {
                input = m_input.pop_front();
                switch (input.Message.at("35")[0])
                {
                    case fix::NewOrderSingle::cMsgType: (void)treatNewOrderSingle(std::move(input));
                        break;
                    case fix::OrderCancelRequest::cMsgType: (void)treatOrderCancelRequest(std::move(input));
                        break;
                    case fix::OrderCancelReplaceRequest::cMsgType: (void)treatOrderCancelReplaceRequest(std::move(input));
                        break;
                    // case fix::MarketDataRequest::cMsgType:
                    //     break;
                    // default: (void)treatUnknown(input);
                    //     break;
                }
            }
        }
    }

    std::string Router::getThreadName() const
    {
        return "Router - " + m_symbol;
    }

    bool Router::treatNewOrderSingle(InputType &&_input)
    {
        Logger::Log("(New Order Single) Processing message...");
        Context<data::OBActionInput> data(_input.Client, _input.ReceiveTime);
        std::pair<bool, fix::Reject> reject = fix::NewOrderSingle::Verify(_input.Message);

        if (reject.first) {
            Logger::Log("(New Order Single) Request verification failed");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger::Log("(Logout) Reject from (", *(_input.Client), ") moving to TCP output");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
            return false;
        }

        data.action = data::OBActionInput::Action::Add;
        data.type = (_input.Message.at(fix::Tag::Side) == "3") ? OrderType::Bid : OrderType::Ask;
        data.price = utils::to<Price>(_input.Message.at(fix::Tag::Price));
        data.order.userId = _input.Client->getUserId();
        data.order.orderId = _input.Message.at(fix::Tag::ClOrdID);
        data.order.quantity = utils::to<Quantity>(_input.Message.at(fix::Tag::OrderQty));
        Logger::Log("(New Order Single) Moving to target OrderBook Container new order: (", data.order.orderId,") from: (", data.order.userId, ")");
        m_ob_action.push(std::move(data));
        return true;
    }

    bool Router::treatOrderCancelRequest(InputType &&_input)
    {
        Logger::Log("(Order Cancel Request) Processing message...");
        Context<data::OBActionInput> data(_input.Client, _input.ReceiveTime);
        std::pair<bool, fix::Reject> reject = fix::OrderCancelRequest::Verify(_input.Message);

        if (reject.first) {
            Logger::Log("(Order Cancel Request) Request verification failed");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger::Log("(Order Cancel Request) Reject from (", *(_input.Client), ") moving to TCP output");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
            return false;
        }
        data.action = data::OBActionInput::Action::Cancel;
        data.order.orderId = _input.Message.at(fix::Tag::OrigClOrdID);
        data.order.userId = _input.Client->getUserId();
        data.type = (_input.Message.at(fix::Tag::Side) == "3") ? OrderType::Bid : OrderType::Ask;
        Logger::Log("(New Order Single) Moving to target OrderBook Container cancel order: (", data.order.orderId,") from: (", data.order.userId, ")");
        m_ob_action.push(std::move(data));
        return true;
    }

    bool Router::treatOrderCancelReplaceRequest(InputType &&_input)
    {
        Logger::Log("(Order Cancel Replace) Processing message...");
        Context<data::OBActionInput> data(_input.Client, _input.ReceiveTime);
        std::pair<bool, fix::Reject> reject = fix::OrderCancelReplaceRequest::Verify(_input.Message);

        if (reject.first) {
            Logger::Log("(Order Cancel Replace) Request verification failed");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger::Log("(Order Cancel Replace) Reject from (", *(_input.Client), ") moving to TCP output");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
            return false;
        }
        data.action = data::OBActionInput::Action::Modify;
        data.order.userId = _input.Client->getUserId();
        data.target = _input.Message.at(fix::Tag::OrigClOrdID);
        data.order.orderId = _input.Message.at(fix::Tag::ClOrdID);
        data.order.quantity = utils::to<Quantity>(_input.Message.at(fix::Tag::OrderQty));
        data.price = utils::to<Price>(_input.Message.at(fix::Tag::Price));
        data.type = (_input.Message.at(fix::Tag::Side) == "3") ? OrderType::Bid : OrderType::Ask;
        Logger::Log("(New Order Replace) Moving to target OrderBook Container replace order: (", data.order.orderId,") from: (", data.order.userId, ")");
        m_ob_action.push(std::move(data));
        return true;
    }
}