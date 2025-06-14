#include "Client/Back/ProcessUnit/FixBuilder.hpp"
#include "Client/Back/User.hpp"

#include "Client/Common/ipc/Header.hpp"

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
        std::string name;
        uint32_t seqnum;
        fix::Logon logon;

        _buffer >> name >> seqnum;
        User::Instance().setSeqNumber(seqnum);
        User::Instance().setUserId(name);
        logon.set108_HeartBtInt("5");
        logon.set98_EncryptMethod("0");
        return logon;
    }
}