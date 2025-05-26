#include <future>

#include "Server/Core/ProcessUnit/Market/OBAction.hpp"
#include "Server/Core/meta.hpp"

#include "Common/Message/ExecutionReport.hpp"
#include "Common/Message/OrderCancelReject.hpp"
#include "Common/Log/Manager.hpp"

namespace pu::market
{
    OBAction::OBAction(OrderBook &_ob, InputNetworkOutput &_output)
        : m_tcp_output(_output), m_ob(_ob), Logger(log::Manager::newLogger("Market/" + m_ob.getSymbol() + "/OB-Event"))
    {
    }

    OBAction::QueueInputType &OBAction::getInput()
    {
        return m_input;
    }

    void OBAction::runtime(std::stop_token _st)
    {
        Logger->log<log::Level::Info>("Starting process unit...");
        InputType input;

        while (!_st.stop_requested()) {
            if (!m_input.empty()) {
                input = m_input.pop_front();
                process(input);
            }
        }
    }

    void OBAction::process(InputType &_data)
    {
        Logger->log<log::Level::Info>("Processing new action: ", _data.Client->getUserId());

        switch (_data.action) {
            case data::OBActionInput::Action::Add:
                m_tp.enqueue([this, data = std::move(_data)] () mutable {
                    runAdd(data);
                });
                break;
            case data::OBActionInput::Action::Modify:
                m_tp.enqueue([this, data = std::move(_data)] () mutable {
                    runModify(data);
                });
                break;
            case data::OBActionInput::Action::Cancel:
                m_tp.enqueue([this, data = std::move(_data)] () mutable {
                    runCancel(data);
                });
                break;
            default:
                // send an internal error message
                return;
        }
    }

    bool OBAction::runAdd(const InputType &_data)
    {
        fix::ExecutionReport report;
        Order order = _data.order;

        Logger->log<log::Level::Info>("(New) request: ", _data.order); // todo log
        if (!m_ob.add(_data.type, _data.price, order)) {
            Logger->log<log::Level::Info>("(New) Reject: Order ID already used: ", _data.order.orderId);
            report.set14_cumQty("0");
            report.set17_execID();
            report.set20_execTransType("1");
            report.set37_orderID(_data.order.orderId);
            report.set38_orderQty(std::to_string(_data.order.quantity));
            report.set39_ordStatus("8");
            report.set40_ordType("2");
            report.set44_price(std::to_string(_data.price));
            report.set54_side((_data.type == OrderType::Ask) ? "4" : "3");
            report.set58_text("Order Id already used");
            report.set151_leavesQty("0");
            m_tcp_output.append(_data.Client, _data.ReceiveTime, std::move(report));
            return false;
        }
        Logger->log<log::Level::Info>("(New) Order executaded sucefully: ", order, ", price: ", _data.price);
        return true;
    }

    bool OBAction::runModify(const InputType &_data)
    {
        fix::OrderCancelReject report;
        Order order = _data.order;

        Logger->log<log::Level::Info>("(Modify) Request: "); // todo log
        report.set37_orderID(_data.target);
        report.set11_clOrdID(_data.target);
        report.set41_origClOrdID(_data.order.orderId);
        if (!m_ob.cancel(_data.type, _data.target, false)) {
            report.set39_ordStatus("8");
            report.set58_text("Order ID doesn't exist");
            Logger->log<log::Level::Info>("(Modify-Cancel) Reject: Order ID already exist: ", _data.target);
            m_tcp_output.append(_data.Client, _data.ReceiveTime, std::move(report));
            return false;
        } else if (!m_ob.modify(_data.type, _data.price, order)) {
            report.set39_ordStatus("4");
            report.set58_text("Order ID already exist, target got canceled");
            Logger->log<log::Level::Info>("(Modify-Add) Reject: Order ID already exist: ", _data.order.orderId);
            m_tcp_output.append(_data.Client, _data.ReceiveTime, std::move(report));
            return false;
        }
        Logger->log<log::Level::Info>("(Modify) Order modify sucessfully: ", _data.target, " -> ", order);
        return true;
    }

    bool OBAction::runCancel(const InputType &_data)
    {
        fix::OrderCancelReject report;

        Logger->log<log::Level::Info>("(Cancel) Request: ", _data.order.orderId);
        if (!m_ob.cancel(_data.type, _data.order.orderId)) {
            Logger->log<log::Level::Info>("(Cancel) Reject: Order ID not found: ", _data.order.orderId);
            report.set11_clOrdID(_data.order.orderId);
            report.set37_orderID(_data.order.orderId);
            report.set41_origClOrdID(_data.order.orderId);
            report.set434_cxlRejReason("8");
            report.set58_text("Order ID doesn't exist");
            m_tcp_output.append(_data.Client, _data.ReceiveTime, std::move(report));
            return false;
        }
        Logger->log<log::Level::Info>("(Cancel) Successfully executed on: ", _data.order.orderId);
        return true;
    }
}