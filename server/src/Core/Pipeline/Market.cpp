#include <future>

#include "Common/Core/Logger.hpp"
#include "Server/Core/Pipeline/Market.hpp"
#include "Server/Core/meta.hpp"
#include "Common/Message/ExecutionReport.hpp"
#include "Common/Message/OrderCancelReject.hpp"

namespace pip
{
    Market::Market(OrderBook &_ob, InOutNetwork &_output)
        : m_tcp_output(_output), m_ob(_ob)
    {
    }

    Market::QueueInputType &Market::getInput()
    {
        return m_input;
    }

    void Market::runtime(std::stop_token _st)
    {
        Context<MarketInput> input;

        while (!_st.stop_requested()) {
            if (!m_input.empty()) {
                input = m_input.pop_front();
                process(input);
            }
        }
    }

    void Market::process(Context<MarketInput> &_data)
    {
        Logger::Log("[Market] Processing new action: ", _data.Client->getUserId());

        switch (_data.OrderData.action) {
            case OrderBook::Data::Action::Add:
                m_tp.enqueue([this, data = std::move(_data)] () mutable {
                    runAdd(data);
                });
                break;
            case OrderBook::Data::Action::Modify:
                m_tp.enqueue([this, data = std::move(_data)] () mutable {
                    runModify(data);
                });
                break;
            case OrderBook::Data::Action::Cancel:
                m_tp.enqueue([this, data = std::move(_data)] () mutable {
                    runCancel(data);
                });
                break;
            default:
                // send an internal error message
                return;
        }
    }

    bool Market::runAdd(const Context<MarketInput> &_data)
    {
        fix::ExecutionReport report;
        Order order = _data.OrderData.order;

        Logger::Log("[Market] (New) request: ", _data.OrderData.order); // todo log
        if (!m_ob.add(_data.OrderData.type, _data.OrderData.price, order)) {
            Logger::Log("[Market] (New) Reject: Order ID already used: ", _data.OrderData.order.orderId);
            report.set14_cumQty("0");
            report.set17_execID();
            report.set20_execTransType("1");
            report.set37_orderID(_data.OrderData.order.orderId);
            report.set38_orderQty(std::to_string(_data.OrderData.order.quantity));
            report.set39_ordStatus("8");
            report.set40_ordType("2");
            report.set44_price(std::to_string(_data.OrderData.price));
            report.set54_side((_data.OrderData.type == OrderType::Ask) ? "4" : "3");
            report.set58_text("Order Id already used");
            report.set151_leavesQty("0");
            m_tcp_output.append(_data.Client, _data.ReceiveTime, std::move(report));
            return false;
        }
        Logger::Log("[Market] (New) Order executaded sucefully: ", order, ", price: ", _data.OrderData.price);
        return true;
    }

    bool Market::runModify(const Context<MarketInput> &_data)
    {
        fix::OrderCancelReject report;
        Order order = _data.OrderData.order;

        Logger::Log("[Market] (Modify) Request: "); // todo log
        report.set37_orderID(_data.OrderData.target);
        report.set11_clOrdID(_data.OrderData.target);
        report.set41_origClOrdID(_data.OrderData.order.orderId);
        if (!m_ob.cancel(_data.OrderData.type, _data.OrderData.target, false)) {
            report.set39_ordStatus("8");
            report.set58_text("Order ID doesn't exist");
            Logger::Log("[Market] (Modify-Cancel) Reject: Order ID already exist: ", _data.OrderData.target);
            m_tcp_output.append(_data.Client, _data.ReceiveTime, std::move(report));
            return false;
        } else if (!m_ob.modify(_data.OrderData.type, _data.OrderData.price, order)) {
            report.set39_ordStatus("4");
            report.set58_text("Order ID already exist, target got canceled");
            Logger::Log("[Market] (Modify-Add) Reject: Order ID already exist: ", _data.OrderData.order.orderId);
            m_tcp_output.append(_data.Client, _data.ReceiveTime, std::move(report));
            return false;
        }
        Logger::Log("[Market] (Modify) Order modify sucessfully: ", _data.OrderData.target, " -> ", order);
        return true;
    }

    bool Market::runCancel(const Context<MarketInput> &_data)
    {
        fix::OrderCancelReject report;

        Logger::Log("[Market] (Cancel) Request: ", _data.OrderData.order.orderId);
        if (!m_ob.cancel(_data.OrderData.type, _data.OrderData.order.orderId)) {
            Logger::Log("[Market] (Cancel) Reject: Order ID not found: ", _data.OrderData.order.orderId);
            report.set11_clOrdID(_data.OrderData.order.orderId);
            report.set37_orderID(_data.OrderData.order.orderId);
            report.set41_origClOrdID(_data.OrderData.order.orderId);
            report.set434_cxlRejReason("8");
            report.set58_text("Order ID doesn't exist");
            m_tcp_output.append(_data.Client, _data.ReceiveTime, std::move(report));
            return false;
        }
        Logger::Log("[Market] (Cancel) Successfully executed on: ", _data.OrderData.order.orderId);
        return true;
    }
}