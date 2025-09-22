#include <future>

#include "Server/ProcessUnit/Market/NewOrder.hpp"


#include "Shared/Message-v2/Parser.hpp"
#include "Shared/Log/Manager.hpp"
#include "Shared/Utils/Utils.hpp"

namespace pu::market
{
    NewOrder::NewOrder(OrderBook &_ob, StringOutputQueue &_output)
        : AInputProcess<InputType>("Server/Market/" + _ob.getSymbol() + "/New-order"),
        m_tcp_output(_output), m_ob(_ob)
    {
    }

    void NewOrder::onInput(InputType _input)
    {
        switch (_input.Message.get<fix42::tag::OrdType>().Value) {
            case fix42::OrderType::Limit:
                newOrderLimit(_input);
                break;
            default:
                // tmp
                notSupportedOrderType(_input);
                break;
        }
    }

    void NewOrder::notSupportedOrderType(const InputType &_input)
    {
        fix42::msg::BusinessReject reject{};

        reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
        reject.get<fix42::tag::RefMsgType>().Value = _input.Header.getPositional<fix42::tag::MsgType>().Value;
        reject.get<fix42::tag::BusinessRejectRefId>().Value = _input.Message.get<fix42::tag::ClOrdID>().Value;
        reject.get<fix42::tag::BusinessRejectReason>().Value = fix42::RejectReasonBusiness::Other;
        reject.get<fix42::tag::Text>().Value = "Not supported order type";
        m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::BusinessReject::Type, std::move(reject.to_string()));
    }

    void NewOrder::newOrderLimit(const InputType &_input)
    {
        if (!_input.Message.get<fix42::tag::Price>().Value.has_value()) {
            fix42::msg::BusinessReject reject{};

            reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
            reject.get<fix42::tag::RefMsgType>().Value = _input.Header.getPositional<fix42::tag::MsgType>().Value;
            reject.get<fix42::tag::BusinessRejectRefId>().Value = _input.Message.get<fix42::tag::ClOrdID>().Value;
            reject.get<fix42::tag::BusinessRejectReason>().Value = fix42::RejectReasonBusiness::CondReqFieldMissing;
            reject.get<fix42::tag::Text>().Value = "Price required when OrderType is Limit";
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::BusinessReject::Type, std::move(reject.to_string()));
            return;
        } else if (!_input.Message.get<fix42::tag::OrderQty>().Value.has_value()) {
            fix42::msg::BusinessReject reject{};

            reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
            reject.get<fix42::tag::RefMsgType>().Value = _input.Header.getPositional<fix42::tag::MsgType>().Value;
            reject.get<fix42::tag::BusinessRejectRefId>().Value = _input.Message.get<fix42::tag::ClOrdID>().Value;
            reject.get<fix42::tag::BusinessRejectReason>().Value = fix42::RejectReasonBusiness::CondReqFieldMissing;
            reject.get<fix42::tag::Text>().Value = "Order quantity required";
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::BusinessReject::Type, std::move(reject.to_string()));
            return;
        }

        // tmp
        switch (_input.Message.get<fix42::tag::Side>().Value) {
            case fix42::Side::BuyMinus:
            case fix42::Side::SellPlus:
                break;
            default:
                notSupportedSide(_input);
                return;
        }

        obs::OrderInfo info{};

        info.side = _input.Message.get<fix42::tag::Side>().Value;
        info.price = _input.Message.get<fix42::tag::Price>().Value.value();
        info.execid = utils::Uuid::Generate();
        info.order.userId = _input.Client->getUserId();
        info.order.orderId = _input.Message.get<fix42::tag::ClOrdID>().Value;
        info.order.quantity = _input.Message.get<fix42::tag::OrderQty>().Value.value();

        Logger->log<logger::Level::Info>("New order: ", info.order, " at price: ", info.price, " on side: ", info.side);

        if (!m_ob.has(info.order.orderId)) {
            fix42::msg::ExecutionReport report;

            report.get<fix42::tag::OrderID>().Value = info.order.orderId;
            report.get<fix42::tag::ExecId>().Value = info.execid;
            report.get<fix42::tag::ExecTransType>().Value = fix42::TransactionType::New;
            report.get<fix42::tag::ExecType>().Value = fix42::ExecutionStatus::NewOrder;
            report.get<fix42::tag::OrdStatus>().Value = fix42::OrderStatus::NewOrder;
            report.get<fix42::tag::Symbol>().Value = m_ob.getSymbol();
            report.get<fix42::tag::Side>().Value = info.side;
            report.get<fix42::tag::OrdType>().Value = fix42::OrderType::Limit;
            report.get<fix42::tag::OrderQty>().Value = info.order.quantity;
            report.get<fix42::tag::Price>().Value = info.price;
            report.get<fix42::tag::LeavesQty>().Value = info.order.quantity;
            report.get<fix42::tag::CumQty>().Value = 0.f;
            report.get<fix42::tag::AvgPx>().Value = 0.f;
            Logger->log<logger::Level::Info>("Aknowledge Limit order: ", info.order.orderId, ", with exec Id: ", info.execid);
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::ExecutionReport::Type, std::move(report.to_string()));
            m_ob.add(info);
        } else {
            fix42::msg::BusinessReject reject{};

            reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
            reject.get<fix42::tag::RefMsgType>().Value = _input.Header.getPositional<fix42::tag::MsgType>().Value;
            reject.get<fix42::tag::BusinessRejectRefId>().Value = info.order.orderId;
            reject.get<fix42::tag::BusinessRejectReason>().Value = fix42::RejectReasonBusiness::Other;
            reject.get<fix42::tag::Text>().Value = "Order Id already used";
            Logger->log<logger::Level::Info>("Rejected: Order ID already used: ", info.order);
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::BusinessReject::Type, std::move(reject.to_string()));
        }
    }

    void NewOrder::notSupportedSide(const InputType &_input)
    {
        fix42::msg::BusinessReject reject{};

        reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
        reject.get<fix42::tag::RefMsgType>().Value = _input.Header.getPositional<fix42::tag::MsgType>().Value;
        reject.get<fix42::tag::BusinessRejectRefId>().Value = _input.Message.get<fix42::tag::ClOrdID>().Value;
        reject.get<fix42::tag::BusinessRejectReason>().Value = fix42::RejectReasonBusiness::Other;
        reject.get<fix42::tag::Text>().Value = "Not supported side";
        Logger->log<logger::Level::Warning>("Side not supported");
        m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::BusinessReject::Type, std::move(reject.to_string()));
    }
}