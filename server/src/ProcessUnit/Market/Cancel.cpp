#include "Server/ProcessUnit/Market/Cancel.hpp"

#include "Shared/Message-v2/Message.hpp"
#include "Shared/Utils/Utils.hpp"

namespace pu::market
{
    Cancel::Cancel(QueueMutex<ProcessId> &_mutex, OrderBook &_ob, StringOutputQueue &_output)
        : AInputProcess<InputType>("Server/Market/" + _ob.getSymbol() + "/Cancel"),
        m_mutex(_mutex), m_tcp_output(_output), m_ob(_ob)
    {
    }

    void Cancel::onInput(InputType _input)
    {
        const OrderId &cancel_id = _input.Message.get<fix42::tag::ClOrdID>().Value;
        const OrderId &origin_id = _input.Message.get<fix42::tag::OrigClOrdID>().Value;
        const fix42::Side side = _input.Message.get<fix42::tag::Side>().Value;

        m_ob.lockReadOrder(side);
        if (!m_ob.has(origin_id, side)) {
            m_ob.unlockReadOrder(side);

            fix42::msg::OrderCancelReject reject{};

            reject.get<fix42::tag::OrderID>().Value = "NONE";
            reject.get<fix42::tag::OrigClOrdID>().Value = cancel_id;
            reject.get<fix42::tag::OrdStatus>().Value = fix42::OrderStatus::Rejected;
            reject.get<fix42::tag::CxlRejResponseTo>().Value = fix42::CancelRejectResponseTo::CancelRequest;
            reject.get<fix42::tag::CxlRejReason>().Value = fix42::CancelRejectReason::UnknownOrderCancel;
            reject.get<fix42::tag::Text>().Value = "Unknow order Id on this side";
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::OrderCancelReject::Type, std::move(reject.to_string()));
            return;
        }
        Order order = m_ob.getOrder(origin_id);
        m_ob.unlockReadOrder(side);

        if (order.side != side) {
            // reject wrong side
        }

        if (m_mutex.front() != ProcessId::OrderCancelRequest)
            aknowledgeCancel(order, _input);

        m_mutex.lock(ProcessId::OrderCancelRequest);

        if (!m_ob.has(origin_id, side)) {
            m_mutex.unlock();
            rejectOrderAlreadyProcess(origin_id, cancel_id);
            return;
        }
        order = m_ob.getOrder(origin_id);
        if (order.remainQty != _input.Message.get<fix42::tag::OrderQty>().Value) {
            m_mutex.unlock();
            rejectOrderInvalidQty(origin_id, cancel_id);
            return;
        }
        if (m_ob.cancel(cancel_id, _input.Message.get<fix42::tag::Side>().Value, )) {
            m_mutex.unlock();

            fix42::msg::ExecutionReport report;

            report.get<fix42::tag::OrderID>().Value = origin_id;
            report.get<fix42::tag::ClOrdID>().Value = cancel_id;
            report.get<fix42::tag::ExecId>().Value = utils::Uuid::Generate();
            report.get<fix42::tag::ExecTransType>().Value = fix42::TransactionType::Cancel;
            report.get<fix42::tag::ExecType>().Value = fix42::ExecutionStatus::Canceled;
            report.get<fix42::tag::OrdStatus>().Value = order.status;
            report.get<fix42::tag::Symbol>().Value = m_ob.getSymbol();
            report.get<fix42::tag::LeavesQty>().Value = order.remainQty;
            report.get<fix42::tag::CumQty>().Value = order.originalQty - order.remainQty;
            report.get<fix42::tag::AvgPx>().Value = order.avgPrice;
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::ExecutionReport::Type, std::move(report.to_string()));
        } else {
            m_fair.unlock();

            fix42::msg::OrderCancelReject reject{};

            reject.get<fix42::tag::OrderID>().Value = origin_id;
            reject.get<fix42::tag::OrigClOrdID>().Value = cancel_id;
            reject.get<fix42::tag::OrdStatus>().Value = order.status;
            reject.get<fix42::tag::CxlRejResponseTo>().Value = fix42::CancelRejectResponseTo::CancelRequest;
            reject.get<fix42::tag::CxlRejReason>().Value = fix42::CancelRejectReason::UnknownOrderCancel;
            reject.get<fix42::tag::Text>().Value = "No matching order to cancel";
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::OrderCancelReject::Type, std::move(reject.to_string()));
        }
    }

    void Cancel::aknowledgeCancel(InputType _input)
    {
        fix42::msg::ExecutionReport report;

        report.get<fix42::tag::OrderID>().Value = _input.Message.get<fix42::tag::OrigClOrdID>().Value;
        report.get<fix42::tag::ClOrdID>().Value = _input.Message.get<fix42::tag::ClOrdID>().Value;
        report.get<fix42::tag::ExecId>().Value = utils::Uuid::Generate();
        report.get<fix42::tag::ExecTransType>().Value = fix42::TransactionType::Cancel;
        report.get<fix42::tag::ExecType>().Value = fix42::ExecutionStatus::PendingCancel;
        report.get<fix42::tag::OrdStatus>().Value = order.status;
        report.get<fix42::tag::Symbol>().Value = m_ob.getSymbol();
        report.get<fix42::tag::LeavesQty>().Value = order.remainQty;
        report.get<fix42::tag::CumQty>().Value = order.originalQty - order.remainQty;
        report.get<fix42::tag::AvgPx>().Value = order.avgPrice;
        m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::ExecutionReport::Type, std::move(report.to_string()));
    }

    void rejectOrderAlreadyProcess(const OrderId &_origin, const OrderId &_cancel)
    {
        fix42::msg::OrderCancelReject reject{};

        reject.get<fix42::tag::OrderID>().Value = _origin;
        reject.get<fix42::tag::OrigClOrdID>().Value = _cancel;
        reject.get<fix42::tag::OrdStatus>().Value = fix42::OrderStatus::Rejected;
        reject.get<fix42::tag::CxlRejResponseTo>().Value = fix42::CancelRejectResponseTo::CancelRequest;
        reject.get<fix42::tag::CxlRejReason>().Value = fix42::CancelRejectReason::TooLateCancel;
        reject.get<fix42::tag::Text>().Value = "Order already processed";
        m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::OrderCancelReject::Type, std::move(reject.to_string()));
    }

    void rejectOrderInvalidQty(const OrderId &_origin, const OrderId &_cancel)
    {
        fix42::msg::OrderCancelReject reject{};

        reject.get<fix42::tag::OrderID>().Value = _origin;
        reject.get<fix42::tag::OrigClOrdID>().Value = _cancel;
        reject.get<fix42::tag::OrdStatus>().Value = fix42::OrderStatus::Rejected;
        reject.get<fix42::tag::CxlRejResponseTo>().Value = fix42::CancelRejectResponseTo::CancelRequest;
        reject.get<fix42::tag::CxlRejReason>().Value = fix42::CancelRejectReason::TooLateCancel;
        reject.get<fix42::tag::Text>().Value = "ORder has invalid quantity";
        m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::OrderCancelReject::Type, std::move(reject.to_string()));
    }
}