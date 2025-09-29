#include "Server/ProcessUnit/Market/Cancel.hpp"

#include "Shared/Message-v2/Message.hpp"
#include "Shared/Utils/Utils.hpp"

namespace pu::market
{
    Cancel::Cancel(QueueMutex<ExecId> &_mutex, OrderBook &_ob, StringOutputQueue &_output)
        : AInputProcess<InputType>("Server/Market/" + _ob.getSymbol() + "/Cancel"),
        m_mutex(_mutex), m_tcp_output(_output), m_ob(_ob)
    {
    }

    void Cancel::setup()
    {
        m_thread = std::jthread(&Cancel::orderProcessing, this);
    }

    void Cancel::onInput(InputType _input)
    {

        if (!_input.Message.get<fix42::tag::OrderQty>().Value.has_value()) {
            m_mutex.finish(_input.ExecutionId);
            fix42::msg::BusinessReject reject{};

            Logger->log<logger::Level::Info>("Order cancel rejected because no quantity was provided");
            reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
            reject.get<fix42::tag::RefMsgType>().Value = _input.Header.getPositional<fix42::tag::MsgType>().Value;
            reject.get<fix42::tag::BusinessRejectRefId>().Value = _input.Message.get<fix42::tag::ClOrdID>().Value;
            reject.get<fix42::tag::BusinessRejectReason>().Value = fix42::RejectReasonBusiness::CondReqFieldMissing;
            reject.get<fix42::tag::Text>().Value = "Quantity is required";
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::BusinessReject::Type, std::move(reject.to_string()));
            return;
        }

        std::optional<Order> order = verifyOrderState(_input);

        if (!order.has_value()) {
            m_mutex.finish(_input.ExecutionId);
            Logger->log<logger::Level::Verbose>("Verification of the order state failed");
            return;
        }

        Logger->log<logger::Level::Verbose>("Mutex Queue front value: ", m_mutex.front());
        if (!m_internal_queue.empty() || m_mutex.front() != _input.ExecutionId) {
            fix42::msg::ExecutionReport report;

            Logger->log<logger::Level::Info>("Aknowledging the cancel request");
            report.get<fix42::tag::OrderID>().Value = _input.Message.get<fix42::tag::OrigClOrdID>().Value;
            report.get<fix42::tag::ClOrdID>().Value = _input.Message.get<fix42::tag::ClOrdID>().Value;
            report.get<fix42::tag::ExecId>().Value = _input.ExecutionId;
            report.get<fix42::tag::ExecTransType>().Value = fix42::TransactionType::Cancel;
            report.get<fix42::tag::ExecType>().Value = fix42::ExecutionStatus::PendingCancel;
            report.get<fix42::tag::OrdStatus>().Value = order.value().status;
            report.get<fix42::tag::Symbol>().Value = m_ob.getSymbol();
            report.get<fix42::tag::LeavesQty>().Value = order.value().remainQty;
            report.get<fix42::tag::CumQty>().Value = order.value().originalQty - order.value().remainQty;
            report.get<fix42::tag::AvgPx>().Value = order.value().avgPrice;
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::ExecutionReport::Type, std::move(report.to_string()));
        }

        m_internal_queue.push(std::move(_input));
    }

    void Cancel::onStop()
    {
        if (m_thread.joinable()) {
            Logger->log<logger::Level::Info>("Requesting stop on the worker thread");
            m_thread.request_stop();
            m_thread.join();
            Logger->log<logger::Level::Debug>("Worker thread joined");
        }
    }

    void Cancel::orderProcessing(std::stop_token _st)
    {
        Logger->log<logger::Level::Info>("Entering worker thread");
        while (!_st.stop_requested()) {
            while (!m_internal_queue.empty()) {
                InputType input = m_internal_queue.pop_front();
                const OrderId &cancel_id = input.Message.get<fix42::tag::ClOrdID>().Value;
                const OrderId &origin_id = input.Message.get<fix42::tag::OrigClOrdID>().Value;

                Logger->log<logger::Level::Debug>("Entering in lock state queue mutex");
                m_mutex.lock(input.ExecutionId);
                Logger->log<logger::Level::Debug>("Lock acquired from queue mutex");

                std::optional<Order> order = verifyOrderStateWithLock(input);

                if (!order.has_value())
                    return;
                if (m_ob.cancel(cancel_id, input.Message.get<fix42::tag::Side>().Value)) {
                    m_mutex.unlock();

                    fix42::msg::ExecutionReport report;

                    Logger->log<logger::Level::Info>("Cancel on order: ", cancel_id, " has been successfull");
                    report.get<fix42::tag::OrderID>().Value = origin_id;
                    report.get<fix42::tag::ClOrdID>().Value = cancel_id;
                    report.get<fix42::tag::ExecId>().Value = input.ExecutionId;
                    report.get<fix42::tag::ExecTransType>().Value = fix42::TransactionType::Cancel;
                    report.get<fix42::tag::ExecType>().Value = fix42::ExecutionStatus::Canceled;
                    report.get<fix42::tag::OrdStatus>().Value = order.value().status;
                    report.get<fix42::tag::Symbol>().Value = m_ob.getSymbol();
                    report.get<fix42::tag::LeavesQty>().Value = order.value().remainQty;
                    report.get<fix42::tag::CumQty>().Value = order.value().originalQty - order.value().remainQty;
                    report.get<fix42::tag::AvgPx>().Value = order.value().avgPrice;
                    m_tcp_output.append(input.Client, input.ReceiveTime, fix42::msg::ExecutionReport::Type, std::move(report.to_string()));
                } else {
                    m_mutex.unlock();

                    fix42::msg::OrderCancelReject reject{};

                    Logger->log<logger::Level::Info>("Cancel on order: ", cancel_id, " failed");
                    reject.get<fix42::tag::OrderID>().Value = origin_id;
                    reject.get<fix42::tag::OrigClOrdID>().Value = cancel_id;
                    reject.get<fix42::tag::OrdStatus>().Value = order.value().status;
                    reject.get<fix42::tag::CxlRejResponseTo>().Value = fix42::CancelRejectResponseTo::CancelRequest;
                    reject.get<fix42::tag::CxlRejReason>().Value = fix42::CancelRejectReason::UnknownOrderCancel;
                    reject.get<fix42::tag::Text>().Value = "No matching order to cancel";
                    m_tcp_output.append(input.Client, input.ReceiveTime, fix42::msg::OrderCancelReject::Type, std::move(reject.to_string()));
                }
            }
        }
    }

    std::optional<Order> Cancel::verifyOrderState(const InputType &_input)
    {
        const fix42::Side side = _input.Message.get<fix42::tag::Side>().Value;

        m_ob.lockReadOrder(side);
        if (!m_ob.has(_input.Message.get<fix42::tag::OrigClOrdID>().Value, side)) {
            m_ob.unlockReadOrder(side);

            fix42::msg::OrderCancelReject reject{};

            Logger->log<logger::Level::Debug>("Unknown order ID: ", _input.Message.get<fix42::tag::OrigClOrdID>().Value);
            reject.get<fix42::tag::OrderID>().Value = "NONE";
            reject.get<fix42::tag::OrigClOrdID>().Value = _input.Message.get<fix42::tag::ClOrdID>().Value;
            reject.get<fix42::tag::OrdStatus>().Value = fix42::OrderStatus::Rejected;
            reject.get<fix42::tag::CxlRejResponseTo>().Value = fix42::CancelRejectResponseTo::CancelRequest;
            reject.get<fix42::tag::CxlRejReason>().Value = fix42::CancelRejectReason::UnknownOrderCancel;
            reject.get<fix42::tag::Text>().Value = "Unknow order Id on this side";
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::OrderCancelReject::Type, std::move(reject.to_string()));
            return std::nullopt;
        }
        Logger->log<logger::Level::Verbose>("Valid order ID, getting the order from order book");
        Order order = m_ob.getOrder(_input.Message.get<fix42::tag::OrigClOrdID>().Value);
        m_ob.unlockReadOrder(side);

        if (order.side != side) {
            Logger->log<logger::Level::Info>("Side doesn't exactly match");
            // reject wrong side
            return std::nullopt;
        }
        return order;
    }

    std::optional<Order> Cancel::verifyOrderStateWithLock(const InputType &_input)
    {
        const OrderId &cancel_id = _input.Message.get<fix42::tag::ClOrdID>().Value;
        const OrderId &origin_id = _input.Message.get<fix42::tag::OrigClOrdID>().Value;
        const Quantity qty = _input.Message.get<fix42::tag::OrderQty>().Value.value();

        if (!m_ob.has(origin_id, _input.Message.get<fix42::tag::Side>().Value)) {
            Logger->log<logger::Level::Debug>("Leaving lock state of access mutex");
            m_mutex.unlock();

            fix42::msg::OrderCancelReject reject{};

            Logger->log<logger::Level::Info>("Cancel rejected: order not found");
            reject.get<fix42::tag::OrderID>().Value = origin_id;
            reject.get<fix42::tag::OrigClOrdID>().Value = cancel_id;
            reject.get<fix42::tag::OrdStatus>().Value = fix42::OrderStatus::Rejected;
            reject.get<fix42::tag::CxlRejResponseTo>().Value = fix42::CancelRejectResponseTo::CancelRequest;
            reject.get<fix42::tag::CxlRejReason>().Value = fix42::CancelRejectReason::TooLateCancel;
            reject.get<fix42::tag::Text>().Value = "Order already processed";
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::OrderCancelReject::Type, std::move(reject.to_string()));
            return std::nullopt;
        }

        Order order = m_ob.getOrder(origin_id);

        if (order.remainQty != qty) {
            Logger->log<logger::Level::Debug>("Leaving lock state of access mutex");
            m_mutex.unlock();

            Logger->log<logger::Level::Info>("Cancel rejected: quantity not matching: ", order.remainQty, " != ", qty);
            fix42::msg::OrderCancelReject reject{};

            reject.get<fix42::tag::OrderID>().Value = origin_id;
            reject.get<fix42::tag::OrigClOrdID>().Value = cancel_id;
            reject.get<fix42::tag::OrdStatus>().Value = fix42::OrderStatus::Rejected;
            reject.get<fix42::tag::CxlRejResponseTo>().Value = fix42::CancelRejectResponseTo::CancelRequest;
            reject.get<fix42::tag::CxlRejReason>().Value = fix42::CancelRejectReason::TooLateCancel;
            reject.get<fix42::tag::Text>().Value = "Order has invalid quantity";
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::OrderCancelReject::Type, std::move(reject.to_string()));
            return std::nullopt;
        }
        return order;
    }
}