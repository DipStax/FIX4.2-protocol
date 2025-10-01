#include <future>

#include "Server/ProcessUnit/Market/NewOrder.hpp"


#include "Shared/Message-v2/Parser.hpp"
#include "Shared/Log/Manager.hpp"
#include "Shared/Utils/Utils.hpp"

namespace pu::market
{
    NewOrder::NewOrder(QueueMutex<ExecId> &_mutex, OrderBook &_ob, StringOutputQueue &_output)
        : AInputProcess<InputType>("Server/Market/" + _ob.getSymbol() + "/New-order"),
        m_mutex(_mutex), m_ob(_ob), m_tcp_output(_output)
    {
    }

    void NewOrder::setup()
    {
        m_thread = std::jthread(&NewOrder::orderProcessing, this);
    }

    void NewOrder::onInput(InputType _input)
    {
        if (!_input.Message.get<fix42::tag::Price>().Value.has_value()) {
            m_mutex.finish(_input.ExecutionId);

            fix42::msg::BusinessReject reject{};

            reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
            reject.get<fix42::tag::RefMsgType>().Value = _input.Header.getPositional<fix42::tag::MsgType>().Value;
            reject.get<fix42::tag::BusinessRejectRefId>().Value = _input.Message.get<fix42::tag::ClOrdID>().Value;
            reject.get<fix42::tag::BusinessRejectReason>().Value = fix42::RejectReasonBusiness::CondReqFieldMissing;
            reject.get<fix42::tag::Text>().Value = "Price required when OrderType is Limit";
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::BusinessReject::Type, std::move(reject.to_string()));
            return;
        } else if (!_input.Message.get<fix42::tag::OrderQty>().Value.has_value()) {
            m_mutex.finish(_input.ExecutionId);

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
        switch (_input.Message.get<fix42::tag::OrdType>().Value) {
            case fix42::OrderType::Limit:
                break;
            default:
                notSupportedOrderType(_input);
                m_mutex.finish(_input.ExecutionId);
                break;
        }

        // tmp
        switch (_input.Message.get<fix42::tag::Side>().Value) {
            case fix42::Side::Buy:
            case fix42::Side::BuyMinus:
            case fix42::Side::Sell:
            case fix42::Side::SellPlus:
                break;
            default:
                notSupportedSide(_input);
                m_mutex.finish(_input.ExecutionId);
                return;
        }
        m_internal_queue.push(std::move(_input));
    }

    void NewOrder::onStop()
    {
        if (m_thread.joinable()) {
            Logger->log<logger::Level::Info>("Requesting stop on the worker thread");
            m_thread.request_stop();
            m_thread.join();
            Logger->log<logger::Level::Debug>("Worker thread joined");
        }
    }

    void NewOrder::orderProcessing(std::stop_token _st)
    {
        while (!_st.stop_requested()) {
            while (!m_internal_queue.empty()) {
                InputType input = m_internal_queue.pop_front();

                OrderBook::OrderInfo info{};

                info.price = input.Message.get<fix42::tag::Price>().Value.value();
                info.execid = input.ExecutionId;
                info.order.userId = input.Client->getUserId();
                info.order.orderId = input.Message.get<fix42::tag::ClOrdID>().Value;
                info.order.originalQty = input.Message.get<fix42::tag::OrderQty>().Value.value();
                info.order.remainQty = info.order.originalQty;
                info.order.avgPrice = 0.f;
                info.order.side = input.Message.get<fix42::tag::Side>().Value;
                info.order.status = fix42::OrderStatus::NewOrder;

                Logger->log<logger::Level::Info>("New order: ", info.order, " at price: ", info.price, " on side: ", info.order.side);

                Logger->log<logger::Level::Debug>("Entering in lock state queue mutex");
                m_mutex.lock(input.ExecutionId);
                Logger->log<logger::Level::Debug>("Lock acquired from queue mutex");

                if (!m_ob.allowTick(info.order.side)) {
                    Logger->log<logger::Level::Debug>("Leaving lock state of access mutex");
                    m_mutex.unlock();

                    fix42::msg::ExecutionReport report;

                    report.get<fix42::tag::OrderID>().Value = info.order.orderId;
                    report.get<fix42::tag::ExecId>().Value = info.execid;
                    report.get<fix42::tag::ExecTransType>().Value = fix42::TransactionType::New;
                    report.get<fix42::tag::ExecType>().Value = fix42::ExecutionStatus::Rejected;
                    report.get<fix42::tag::OrdStatus>().Value = fix42::OrderStatus::Rejected;
                    report.get<fix42::tag::Symbol>().Value = m_ob.getSymbol();
                    report.get<fix42::tag::Side>().Value = info.order.side;
                    report.get<fix42::tag::OrderQty>().Value = info.order.originalQty;
                    report.get<fix42::tag::OrdType>().Value = fix42::OrderType::Limit;
                    report.get<fix42::tag::Price>().Value = info.price;
                    report.get<fix42::tag::LeavesQty>().Value = 0.f;
                    report.get<fix42::tag::LastShares>().Value = 0.f;
                    report.get<fix42::tag::LastPx>().Value = 0.f;
                    report.get<fix42::tag::CumQty>().Value = 0.f;
                    report.get<fix42::tag::AvgPx>().Value = 0.f;
                    report.get<fix42::tag::Text>().Value = "Invalid tick direction for selected side";
                    m_tcp_output.append(input.Client, input.ReceiveTime, fix42::msg::ExecutionReport::Type, std::move(report.to_string()));
                } else if (!m_ob.has(info.order.orderId)) {
                    fix42::msg::ExecutionReport report;

                    report.get<fix42::tag::OrderID>().Value = info.order.orderId;
                    report.get<fix42::tag::ExecId>().Value = info.execid;
                    report.get<fix42::tag::ExecTransType>().Value = fix42::TransactionType::New;
                    report.get<fix42::tag::ExecType>().Value = fix42::ExecutionStatus::NewOrder;
                    report.get<fix42::tag::OrdStatus>().Value = fix42::OrderStatus::NewOrder;
                    report.get<fix42::tag::Symbol>().Value = m_ob.getSymbol();
                    report.get<fix42::tag::Side>().Value = info.order.side;
                    report.get<fix42::tag::OrdType>().Value = fix42::OrderType::Limit;
                    report.get<fix42::tag::OrderQty>().Value = info.order.originalQty;
                    report.get<fix42::tag::Price>().Value = info.price;
                    report.get<fix42::tag::LeavesQty>().Value = info.order.remainQty;
                    report.get<fix42::tag::LastShares>().Value = 0.f;
                    report.get<fix42::tag::LastPx>().Value = 0.f;
                    report.get<fix42::tag::CumQty>().Value = 0.f;
                    report.get<fix42::tag::AvgPx>().Value = 0.f;
                    Logger->log<logger::Level::Info>("Aknowledge Limit order: ", info.order.orderId, ", with exec Id: ", info.execid);
                    m_tcp_output.append(input.Client, input.ReceiveTime, fix42::msg::ExecutionReport::Type, std::move(report.to_string()));
                    m_ob.add(info);

                    Logger->log<logger::Level::Debug>("Leaving lock state of access mutex");
                    m_mutex.unlock();
                } else {
                    Logger->log<logger::Level::Debug>("Leaving lock state of access mutex");
                    m_mutex.unlock();

                    fix42::msg::ExecutionReport report;

                    report.get<fix42::tag::OrderID>().Value = info.order.orderId;
                    report.get<fix42::tag::ExecId>().Value = info.execid;
                    report.get<fix42::tag::ExecTransType>().Value = fix42::TransactionType::New;
                    report.get<fix42::tag::ExecType>().Value = fix42::ExecutionStatus::Rejected;
                    report.get<fix42::tag::OrdStatus>().Value = fix42::OrderStatus::Rejected;
                    report.get<fix42::tag::OrdRejReason>().Value = fix42::OrderRejectReason::Duplicate;
                    report.get<fix42::tag::Symbol>().Value = m_ob.getSymbol();
                    report.get<fix42::tag::Side>().Value = info.order.side;
                    report.get<fix42::tag::OrderQty>().Value = info.order.originalQty;
                    report.get<fix42::tag::OrdType>().Value = fix42::OrderType::Limit;
                    report.get<fix42::tag::Price>().Value = info.price;
                    report.get<fix42::tag::LeavesQty>().Value = 0.f;
                    report.get<fix42::tag::LastShares>().Value = 0.f;
                    report.get<fix42::tag::LastPx>().Value = 0.f;
                    report.get<fix42::tag::CumQty>().Value = 0.f;
                    report.get<fix42::tag::AvgPx>().Value = 0.f;
                    report.get<fix42::tag::Text>().Value = "Order Id already used";
                    Logger->log<logger::Level::Info>("Rejected: Order ID already used: ", info.order);
                    m_tcp_output.append(input.Client, input.ReceiveTime, fix42::msg::ExecutionReport::Type, std::move(report.to_string()));
                }
            }
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

    void NewOrder::notSupportedSide(const InputType &_input)
    {
        m_mutex.finish(_input.ExecutionId);
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