#include "Client/Back/ProcessUnit/FixBuilder.hpp"
#include "Client/Back/User.hpp"

#include "Client/Common/IPC/Header.hpp"
#include "Client/Common/IPC/Message/Message.hpp"

#include "Shared/Log/Manager.hpp"

namespace pu
{
    FixBuilder::FixBuilder(ts::Queue<net::Buffer> &_input, QueueMessage &_output)
        : AProcessUnitBase("Client/FixBuilder"),
        m_input(_input), m_output(_output)
    {
    }

    void FixBuilder::runtime(std::stop_token _st)
    {
        Logger->log<logger::Level::Info>("Starting process unit...");
        ipc::Header header;

        while (!_st.stop_requested()) {
            if (!m_input.empty()) {
                net::Buffer buffer = m_input.pop_front();

                buffer >> header;
                switch (header.MsgType)
                {
                    case ipc::MessageType::Logon: m_output.push(buildLogon(buffer));
                        break;
                    case ipc::MessageType::OrderSingle: m_output.push(buildOrderSingle(buffer));
                        break;
                    default:
                        Logger->log<logger::Level::Error>("Unknow message type");
                        break;
                }
            }
        }
    }

    fix::Message FixBuilder::buildLogon(net::Buffer &_buffer)
    {
        fix::Logon logon;
        ipc::msg::Logon ipc_logon;

        _buffer >> ipc_logon;
        Logger->log<logger::Level::Info>("Building Logon message with: ", ipc_logon);
        User::Instance().setSeqNumber(ipc_logon.SeqNum);
        User::Instance().setUserId(ipc_logon.UserId);
        logon.set108_HeartBtInt(std::to_string(ipc_logon.HeartBeat));
        logon.set98_EncryptMethod("0");
        return logon;
    }

    fix::Message FixBuilder::buildOrderSingle(net::Buffer &_buffer)
    {
        fix::NewOrderSingle order;
        ipc::msg::OrderSingle ipc_order;

        _buffer >> ipc_order;
        Logger->log<logger::Level::Info>("Building NewOrderSingle message with: ", ipc_order);
        order.set11_clOrdID(ipc_order.orderId);
        order.set21_handlInst("3");
        order.set38_orderQty(std::to_string(ipc_order.quantity));
        order.set40_ordType("2");
        order.set44_price(std::to_string(ipc_order.price));
        order.set54_side((ipc_order.type == OrderType::Ask) ? "4" : "3");
        order.set55_symbol(ipc_order.symbol);
        return order;
    }
}