#include "Server/ProcessUnit/Market/Cancel.hpp"

#include "Shared/Message-v2/Message.hpp"
#include "Shared/Utils/Utils.hpp"

namespace pu::market
{
    Cancel::Cancel(OrderBook &_ob, StringOutputQueue &_output)
        : AInputProcess<InputType>("Server/Market/" + _ob.getSymbol() + "/Cancel"),
        m_tcp_output(_output), m_ob(_ob)
    {
    }

    void Cancel::onInput(InputType _input)
    {
        const std::string execId = utils::Uuid::Generate();

        if (!m_ob.has(_input.Message.get<fix42::tag::OrigClOrdID>().Value, _input.Message.get<fix42::tag::Side>().Value)) {
            fix42::msg::OrderCancelReject reject{};

            reject.get<fix42::tag::OrderID>().Value = "NONE";
            reject.get<fix42::tag::OrigClOrdID>().Value = _input.Message.get<fix42::tag::ClOrdID>().Value;
            reject.get<fix42::tag::OrdStatus>().Value = fix42::OrderStatus::Rejected;
            reject.get<fix42::tag::CxlRejResponseTo>().Value = fix42::CancelRejectResponseTo::CancelRequest;
            reject.get<fix42::tag::CxlRejReason>().Value = fix42::CancelRejectReason::UnknownOrderCancel;
            reject.get<fix42::tag::Text>().Value = "Unknow order Id on this side";
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::OrderCancelReject::Type, std::move(reject.to_string()));
            return;
        }
        fix42::msg::ExecutionReport report;

        // can throw an exception if between the has and the getOrder the order is filled
        Order order = m_ob.getOrder(_input.Message.get<fix42::tag::OrigClOrdID>().Value);

        report.get<fix42::tag::OrderID>().Value = _input.Message.get<fix42::tag::ClOrdID>().Value;
        report.get<fix42::tag::ExecId>().Value = execId;
        report.get<fix42::tag::ExecTransType>().Value = fix42::TransactionType::Cancel;
        report.get<fix42::tag::ExecType>().Value = fix42::ExecutionStatus::PendingCancel;
        report.get<fix42::tag::OrdStatus>().Value = order.status;
        report.get<fix42::tag::Symbol>().Value = m_ob.getSymbol();
        report.get<fix42::tag::LeavesQty>().Value = order.remainQty;
        report.get<fix42::tag::CumQty>().Value = order.originalQty - order.remainQty;
        report.get<fix42::tag::AvgPx>().Value = order.avgPrice;
        m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::ExecutionReport::Type, std::move(report.to_string()));
    }
}