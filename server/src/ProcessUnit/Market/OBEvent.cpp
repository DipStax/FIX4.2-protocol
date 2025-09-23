#include "Server/ProcessUnit/Market/OBEvent.hpp"

#include "Shared/Utils/Utils.hpp"
#include "Shared/Message-v2/Message.hpp"

namespace pu::market
{
    OBEvent::OBEvent(const std::string &_symbol, StringOutputQueue &_tcp_output)
        : AInputProcess<InputType>("Market/" + _symbol + "/OB-Event"),
        m_symbol(_symbol), m_tcp_output(_tcp_output)
    {
    }


    void OBEvent::onInput(InputType _input)
    {
        m_tp.enqueue([this, _data = std::move(_input)] () {
            createEvent(_data);
        });
    }

    void OBEvent::createEvent(const InputType &_input)
    {
        fix42::msg::ExecutionReport report;
        ClientStore::Client client = ClientStore::Instance().findClient(_input.userId);

        if (client == nullptr)
            return;

        report.get<fix42::tag::OrderID>().Value = _input.orderId;
        report.get<fix42::tag::ExecId>().Value = utils::Uuid::Generate();
        report.get<fix42::tag::ExecTransType>().Value = fix42::TransactionType::New;
        report.get<fix42::tag::ExecType>().Value = _input.execStatus;
        report.get<fix42::tag::OrdStatus>().Value = _input.ordStatus;
        report.get<fix42::tag::Symbol>().Value = m_symbol;
        report.get<fix42::tag::Side>().Value = _input.side;
        report.get<fix42::tag::OrdType>().Value = fix42::OrderType::Limit;
        report.get<fix42::tag::Price>().Value = _input.price;
        report.get<fix42::tag::LeavesQty>().Value = _input.remainQty;
        report.get<fix42::tag::CumQty>().Value = _input.orgQty - _input.remainQty;
        report.get<fix42::tag::AvgPx>().Value = _input.avgPrice;
        report.get<fix42::tag::OrderQty>().Value = _input.orgQty;
        Logger->log<logger::Level::Debug>("Report created for order Id: ", _input.orderId);
        m_tcp_output.append(client, std::chrono::system_clock::now(), fix42::msg::ExecutionReport::Type, std::move(report.to_string()));
    }
}