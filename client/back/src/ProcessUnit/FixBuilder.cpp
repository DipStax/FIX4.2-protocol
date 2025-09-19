#include "Client/Back/ProcessUnit/FixBuilder.hpp"
#include "Client/Back/User.hpp"

#include "Client/Shared/IPC/Header.hpp"
#include "Client/Shared/IPC/Message/Message.hpp"

#include "Shared/Log/Manager.hpp"

namespace pu
{
    FixBuilder::FixBuilder(ts::Queue<net::Buffer> &_input, StringOutputQueue &_output)
        : AProcessUnitBase("Back/FixBuilder"),
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
                    case ipc::MessageType::Logon:
                        m_output.append(std::chrono::system_clock::now(), std::move(buildLogon(buffer)));
                        break;
                    case ipc::MessageType::OrderSingle:
                        m_output.append(std::chrono::system_clock::now(), std::move(buildOrderSingle(buffer)));
                        break;
                    default:
                        Logger->log<logger::Level::Error>("Unknow message type");
                        break;
                }
            }
        }
    }

    data::StringOutput FixBuilder::buildLogon(net::Buffer &_buffer)
    {
        fix42::msg::Logon logon;
        ipc::msg::Logon ipc_logon;

        _buffer >> ipc_logon;
        Logger->log<logger::Level::Info>("Building Logon message with: ", ipc_logon);
        logon.get<fix42::tag::EncryptMethod>().Value = fix42::EncryptionMethod::None;
        logon.get<fix42::tag::HeartBtInt>().Value = ipc_logon.HeartBeat + 1;

        User::Instance().setUserId(ipc_logon.UserId);
        User::Instance().setSeqNumber(ipc_logon.SeqNum);

        return {fix42::msg::Logon::Type, std::move(logon.to_string())};
    }

    data::StringOutput FixBuilder::buildOrderSingle(net::Buffer &_buffer)
    {
        fix42::msg::NewOrderSingle order;
        ipc::msg::OrderSingle ipc_order;

        _buffer >> ipc_order;
        Logger->log<logger::Level::Info>("Building NewOrderSingle message with: ", ipc_order);
        order.get<fix42::tag::ClOrdID>().Value = ipc_order.orderId;
        order.get<fix42::tag::HandlInst>().Value = fix42::HandleInstance::Manual;
        order.get<fix42::tag::OrderQty>().Value = ipc_order.quantity;
        order.get<fix42::tag::OrdType>().Value = fix42::OrderType::Limit;
        order.get<fix42::tag::Price>().Value = ipc_order.price;
        order.get<fix42::tag::Side>().Value = ipc_order.type;
        order.get<fix42::tag::Symbol>().Value = ipc_order.symbol;
        return {fix42::msg::NewOrderSingle::Type, std::move(order.to_string())};
    }
}