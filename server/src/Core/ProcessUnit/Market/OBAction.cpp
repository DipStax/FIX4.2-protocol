#include <future>

#include "Server/Core/ProcessUnit/Market/OBAction.hpp"

#include "Common/Message/Message.hpp"
#include "Common/Log/Manager.hpp"
#include "Common/Utils.hpp"
#include "Common/Message/Tag.hpp"

namespace pu::market
{
    OBAction::OBAction(OrderBook &_ob, InputNetworkOutput &_output)
        : m_tcp_output(_output), m_ob(_ob), m_symbol(m_ob.getSymbol()), Logger(logger::Manager::newLogger("Market/" + m_ob.getSymbol() + "/OB-Event"))
    {
    }

    OBAction::QueueInputType &OBAction::getInput()
    {
        return m_input;
    }

    void OBAction::runtime(std::stop_token _st)
    {
        Logger->log<logger::Level::Info>("Starting process unit...");
        InputType input;

        while (!_st.stop_requested()) {
            if (!m_input.empty()) {
                input = m_input.pop_front();
                switch (input.Message.at("35")[0])
                {
                    case fix::NewOrderSingle::cMsgType: treatNewOrderSingle(input);
                        break;
                    // case fix::OrderCancelRequest::cMsgType:
                    // case fix::OrderCancelReplaceRequest::cMsgType:
                    // case fix::MarketDataRequest::cMsgType:
                    //     break;
                    // default: (void)treatUnknown(input);
                    //     break;
                }
            }
        }
    }

    bool OBAction::treatNewOrderSingle(InputType &_input)
    {
        std::pair<bool, fix::Reject> reject = fix::NewOrderSingle::Verify(_input.Message);

        if (reject.first) {
            Logger->log<logger::Level::Info>("(New Order Single) Request verification failed");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger->log<logger::Level::Debug>("(Logout) Reject from (", *(_input.Client), ") moving to TCP output");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
            return false;
        }

        obs::OrderInfo info;


        info.side = (_input.Message.at(fix::Tag::Side) == "3") ? OrderType::Bid : OrderType::Ask;
        info.price = utils::to<Price>(_input.Message.at(fix::Tag::Price));
        info.order.userId = _input.Client->getUserId();
        info.order.orderId = _input.Message.at(fix::Tag::ClOrdID);
        info.order.quantity = utils::to<Quantity>(_input.Message.at(fix::Tag::OrderQty));

        Logger->log<logger::Level::Info>("(New) New order: ", info.order, " at price: ", info.price, " on side: ", static_cast<int>(info.side)); // todo log

        std::pair<OrderStatus, Quantity> result = m_ob.add(info);
        switch (result.first) {
            case OrderStatus::Filled:
            case OrderStatus::PartiallyFilled:
            case OrderStatus::New:
                orderValidated(_input, info, result);
                break;
            case OrderStatus::Rejected: rejectOrderIdExist(_input, info);
                return false;
            default:
                Logger->log<logger::Level::Warning>("Not supposed to have other OrderStatus");
                return false;
        }
        return true;
    }

    void OBAction::rejectOrderIdExist(InputType &_input, const obs::OrderInfo &_order)
    {
        fix::BusinessMessageReject reject{};

        Logger->log<logger::Level::Info>("Reject: Order ID already used: ", _order.order);
        reject.set58_text("Order Id already used");
        reject.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
        reject.set379_businessRejectRefId(_order.order.orderId);
        reject.set380_businessRejectReason(fix::BusinessMessageReject::UnknowId);
        reject.set372_refMsgType(_input.Message.at(fix::Tag::MsgType));
        m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject));
    }

    void OBAction::orderValidated(InputType &_input, const obs::OrderInfo &_order, const std::pair<OrderStatus, Quantity> &_result)
    {
        fix::ExecutionReport report{};

        report.set14_cumQty(std::to_string(_order.order.quantity - _result.second));
        report.set17_execID();
        report.set20_execTransType("0");
        report.set38_orderQty(std::to_string(_order.order.quantity));
        report.set37_orderID(_order.order.orderId);
        report.set39_ordStatus(std::to_string(static_cast<uint8_t>(_result.first)));
        report.set40_ordType("2");
        report.set44_price(std::to_string(_order.price));
        report.set54_side((_order.side == OrderType::Ask) ? "4" : "3");
        report.set55_symbol(m_symbol);
        report.set151_leavesQty(std::to_string(_result.second));
        report.set150_execType(std::to_string(static_cast<uint8_t>(_result.first)));
        m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(report));
    }

    // bool OBAction::runModify(const InputType &_data)
    // {
    //     fix::OrderCancelReject report;
    //     Order order = _data.order;

    //     Logger->log<logger::Level::Info>("(Modify) Request: "); // todo log
    //     report.set37_orderID(_data.target);
    //     report.set11_clOrdID(_data.target);
    //     report.set41_origClOrdID(_data.order.orderId);
    //     if (!m_ob.cancel(_data.type, _data.target, false)) {
    //         report.set39_ordStatus("8");
    //         report.set58_text("Order ID doesn't exist");
    //         Logger->log<logger::Level::Info>("(Modify-Cancel) Reject: Order ID already exist: ", _data.target);
    //         m_tcp_output.append(_data.Client, _data.ReceiveTime, std::move(report));
    //         return false;
    //     } else if (!m_ob.modify(_data.type, _data.price, order)) {
    //         report.set39_ordStatus("4");
    //         report.set58_text("Order ID already exist, target got canceled");
    //         Logger->log<logger::Level::Info>("(Modify-Add) Reject: Order ID already exist: ", _data.order.orderId);
    //         m_tcp_output.append(_data.Client, _data.ReceiveTime, std::move(report));
    //         return false;
    //     }
    //     Logger->log<logger::Level::Info>("(Modify) Order modify sucessfully: ", _data.target, " -> ", order);
    //     return true;
    // }

    // bool OBAction::runCancel(const InputType &_data)
    // {
    //     fix::OrderCancelReject report;

    //     Logger->log<logger::Level::Info>("(Cancel) Request: ", _data.order.orderId);
    //     if (!m_ob.cancel(_data.type, _data.order.orderId)) {
    //         Logger->log<logger::Level::Info>("(Cancel) Reject: Order ID not found: ", _data.order.orderId);
    //         report.set11_clOrdID(_data.order.orderId);
    //         report.set37_orderID(_data.order.orderId);
    //         report.set41_origClOrdID(_data.order.orderId);
    //         report.set434_cxlRejReason("8");
    //         report.set58_text("Order ID doesn't exist");
    //         m_tcp_output.append(_data.Client, _data.ReceiveTime, std::move(report));
    //         return false;
    //     }
    //     Logger->log<logger::Level::Info>("(Cancel) Successfully executed on: ", _data.order.orderId);
    //     return true;
    // }
}