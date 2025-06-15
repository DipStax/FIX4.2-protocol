#include "Client/Back/ProcessUnit/FixBuilder.hpp"
#include "Client/Back/User.hpp"

#include "Client/Common/IPC/Header.hpp"
#include "Client/Common/IPC/Message/Logon.hpp"

#include "Common/Log/Manager.hpp"

namespace pu
{
    FixBuilder::FixBuilder(ts::Queue<net::Buffer> &_input, QueueMessage &_output)
        : m_input(_input), m_output(_output), Logger(logger::Manager::newLogger("FixBuilder"))
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
        Logger->log<logger::Level::Info>("Building login message with: { userId: ", ipc_logon.UserId, ", seqnum: ", ipc_logon.SeqNum, ", heartbeat: ", ipc_logon.HeartBeat, " }");
        User::Instance().setSeqNumber(ipc_logon.SeqNum);
        User::Instance().setUserId(ipc_logon.UserId);
        logon.set108_HeartBtInt(std::to_string(ipc_logon.HeartBeat));
        logon.set98_EncryptMethod("0");
        return logon;
    }
}