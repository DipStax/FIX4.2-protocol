#include "Server/Core/ProcessUnit/Market/OBEvent.hpp"

#include "Shared/Message/ExecutionReport.hpp"

namespace pu::market
{
    OBEvent::OBEvent(const std::string &_symbol, InputNetworkOutput &_tcp)
        : AInputProcess<obs::Event>("Market/" + _symbol + "/OB-Event"),
        m_symbol(_symbol), m_tcp_output(_tcp)
    {
    }


    void OBEvent::onInput(InputType _input)
    {
        m_tp.enqueue([this, _data = std::move(_input)] () {
            createEvent(_data);
        });
    }

    bool OBEvent::createEvent(const InputType &_input)
    {
        fix::ExecutionReport report;
        ClientStore::Client client = ClientStore::Instance().findClient(_input.userId);

        if (client == nullptr)
            return false;
        report.set6_avgPx(std::to_string(_input.avgPrice));
        report.set14_cumQty(std::to_string(_input.orgQty - _input.remainQty));
        report.set17_execID();
        report.set20_execTransType("0");
        report.set38_orderQty(std::to_string(_input.orgQty));
        report.set37_orderID(_input.orderId);
        report.set39_ordStatus(std::to_string(static_cast<uint8_t>(_input.ordStatus)));
        report.set40_ordType("2");
        report.set44_price(std::to_string(_input.price));
        report.set54_side((_input.side == OrderType::Ask) ? "4" : "3");
        report.set55_symbol(m_symbol);
        report.set150_execType(std::string(1, static_cast<char>(_input.execStatus)));
        report.set151_leavesQty(std::to_string(_input.remainQty));
        m_tcp_output.append(client, std::chrono::system_clock::now(), std::move(report));
        Logger->log<logger::Level::Debug>("[OBEvent] (TCP) Report created: "); // todo log
        return true;
    }
}