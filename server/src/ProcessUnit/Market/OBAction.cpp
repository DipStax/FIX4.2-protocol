#include <future>

#include "Server/ProcessUnit/Market/OBAction.hpp"


#include "Shared/Message-v2/Parser.hpp"
#include "Shared/Log/Manager.hpp"
#include "Shared/Utils/Utils.hpp"

namespace pu::market
{
    OBAction::OBAction(OrderBook &_ob, StringOutputQueue &_output)
        : AInputProcess<InputType>("Server/Market/" + _ob.getSymbol() + "/OB-Action"),
        m_tcp_output(_output), m_ob(_ob)
    {
    }

    void OBAction::onInput(InputType _input)
    {
        switch (_input.Header.getPositional<fix42::tag::MsgType>().Value)
        {
            case fix42::msg::NewOrderSingle::Type:
                treatNewOrderSingle(_input);
                break;
            // case fix::OrderCancelRequest::cMsgType:
            // case fix::OrderCancelReplaceRequest::cMsgType:
            // case fix::MarketDataRequest::cMsgType:
            //     break;
            // default: (void)treatUnknown(input);
            //     break;
        }
    }

    void OBAction::treatNewOrderSingle(InputType &_input)
    {
        xstd::Expected<fix42::msg::NewOrderSingle, fix42::msg::SessionReject> error = fix42::parseMessage<fix42::msg::NewOrderSingle>(_input.Message, _input.Header);

        if (error.has_error()) {
            Logger->log<logger::Level::Info>("Parsing of NewOrderSingle message failed: ", error.error().get<fix42::tag::Text>().Value.value());
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(error.error().to_string()));
            return;
        }
        const fix42::msg::NewOrderSingle &order = error.value();

        switch (order.get<fix42::tag::OrdType>().Value) {
            case fix42::OrderType::Limit:
                newOrderLimit(_input, order);
                break;
            default:
                // todo orderTypeNotSupported(_input);
                break;
        }
    }

    void OBAction::newOrderLimit(const InputType &_input, const fix42::msg::NewOrderSingle &_order)
    {
        if (!_order.get<fix42::tag::Price>().Value.has_value()) {
            fix42::msg::BusinessReject reject{};

            reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
            reject.get<fix42::tag::RefMsgType>().Value = _input.Header.getPositional<fix42::tag::MsgType>().Value;
            reject.get<fix42::tag::BusinessRejectReason>().Value = fix42::RejectReasonBusiness::CondReqFieldMissing;
            reject.get<fix42::tag::Text>().Value = "Price required when OrderType=Limit";
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::BusinessReject::Type, std::move(reject.to_string()));
            return;
        } else if (!_order.get<fix42::tag::OrderQty>().Value.has_value()) {
            fix42::msg::BusinessReject reject{};

            reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
            reject.get<fix42::tag::RefMsgType>().Value = _input.Header.getPositional<fix42::tag::MsgType>().Value;
            reject.get<fix42::tag::BusinessRejectReason>().Value = fix42::RejectReasonBusiness::CondReqFieldMissing;
            reject.get<fix42::tag::Text>().Value = "Order quantity required";
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::BusinessReject::Type, std::move(reject.to_string()));
            return;
        }

        obs::OrderInfo info{};

        info.side = _order.get<fix42::tag::Side>().Value;
        info.price = _order.get<fix42::tag::Price>().Value.value();
        info.execid = utils::Uuid::Generate();
        info.order.userId = _input.Client->getUserId();
        info.order.orderId = _order.get<fix42::tag::ClOrdID>().Value;
        info.order.quantity = _order.get<fix42::tag::OrderQty>().Value.value();

        Logger->log<logger::Level::Info>("New order: ", info.order, " at price: ", info.price, " on side: ", info.side); // todo log
        if (!m_ob.has(info.order.orderId)) {
            fix42::msg::ExecutionReport report;

            report.get<fix42::tag::OrderID>().Value = info.order.orderId;
            report.get<fix42::tag::ExecId>().Value = info.execid;
            report.get<fix42::tag::ExecTransType>().Value = fix42::TransactionType::New;
            report.get<fix42::tag::ExecType>().Value = fix42::OrderStatus::NewOrder;
            report.get<fix42::tag::OrdStatus>().Value = fix42::OrderStatus::NewOrder;
            report.get<fix42::tag::Symbol>().Value = m_ob.getSymbol();
            report.get<fix42::tag::Side>().Value = info.side;
            report.get<fix42::tag::OrdType>().Value = fix42::OrderType::Limit;
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
}