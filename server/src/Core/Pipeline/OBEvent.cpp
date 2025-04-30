#include <cstring>

#include "Server/Core/Pipeline/OBEvent.hpp"
#include "Common/Message/ExecutionReport.hpp"
#include "Server/Core/Pipeline/Naming.hpp"

namespace pip
{
    OBEvent::OBEvent(const std::string &_name, InUDP &_udp, InOutNetwork &_tcp)
        : m_name(_name), m_udp_output(_udp), m_tcp_output(_tcp)
    {
    }

    OBEvent::InputType &OBEvent::getInput()
    {
        return m_input;
    }

    void OBEvent::runtime(std::stop_token _st)
    {
        OrderBook::Event input;

        while (!_st.stop_requested()) {
            if (!m_input.empty()) {
                input = m_input.pop_front();
                Logger::Log("[OBEvent] New event from the OderBook");
                m_tp.enqueue([this, _data = std::move(input)] () {
                    createUdp(_data);
                    createTcp(_data);
                });
            }
        }
    }

    bool OBEvent::createTcp(const OrderBook::Event &_input)
    {
        fix::ExecutionReport report;
        ClientStore::Client client = ClientStore::Instance().findClient(_input.userId);

        if (client == nullptr)
            return false;
        report.set14_cumQty(std::to_string(_input.orgQty - _input.quantity));
        report.set17_execID();
        report.set20_execTransType("0");
        report.set38_orderQty(std::to_string(_input.orgQty));
        report.set37_orderID(_input.orderId);
        report.set39_ordStatus(std::to_string(static_cast<uint8_t>(_input.status)));
        report.set40_ordType("2");
        report.set44_price(std::to_string(_input.price));
        report.set54_side((_input.side == OrderType::Ask) ? "4" : "3");
        report.set55_symbol(m_name);
        report.set151_leavesQty(std::to_string(_input.quantity));
        report.set150_execType(std::to_string(static_cast<uint8_t>(_input.status)));
        m_tcp_output.append(client, std::chrono::system_clock::now(), std::move(report));
        Logger::Log("[OBEvent] (TCP) Report created: "); // todo log
        return true;
    }

    bool OBEvent::createUdp(const OrderBook::Event &_input)
    {
        static uint64_t id = 0;
        data::UDPPackage package;

        if (_input.status == OrderStatus::Pending || _input.quantity == 0)
            return false;
        package.time = 0; // todo
        package.id = 0;
        package.flag = 0;
        package.quantity = 0;
        package.price = 0;
        package.id = id++;
        UDP_FLAG_SET_SOLD(package.flag, _input.sold);
        UDP_FLAG_SET_STATUS(package.flag, _input.status);
        (_input.side == OrderType::Ask) ? UDP_FLAG_SET_ASK(package.flag) : UDP_FLAG_SET_BID(package.flag);
        package.quantity = _input.quantity;
        package.price = _input.price;

        Logger::Log("[OBEvent] (UDP) Setting Symbol");
        std::memset(package.symbol, '0', std::min(m_name.size(), (size_t)MARKET_NAME_MAX_SIZE));
        std::memcpy(package.symbol, m_name.c_str(), std::min(m_name.size(), (size_t)MARKET_NAME_MAX_SIZE));

        Logger::Log("[OBEvent] (UDP) Report created: ", package);
        m_udp_output.append(std::move(package));
        return true;
    }
}