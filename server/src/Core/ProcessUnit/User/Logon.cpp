#include "Server/Core/ProcessUnit/User/Logon.hpp"

#include "Common/Utils.hpp"
#include "Common/Message/Logon.hpp"
#include "Common/Message/Tag.hpp"
#include "Common/Log/Manager.hpp"

namespace pu::user
{
    LogonHandler::LogonHandler(InputNetworkOutput &_tcp_output)
        : m_tcp_output(_tcp_output), Logger(logger::Manager::newLogger("Logon"))
    {
    }

    LogonHandler::QueueInputType &LogonHandler::getInput()
    {
        return m_input;
    }

    void LogonHandler::runtime(std::stop_token _st)
    {
        Logger->log<logger::Level::Info>("Starting process unit...");

        while (!_st.stop_requested()) {
            while (!m_input.empty()) {
                m_tp.enqueue([this, _input = std::move(m_input.pop_front())] () mutable {
                    process(std::move(_input));
                });
            }
        }
        Logger->log<logger::Level::Warning>("Exiting process unit...");
    }

    bool LogonHandler::process(InputType &&_input)
    {
        Logger->log<logger::Level::Info>("Processing message...");
        fix::Logon logon;
        std::pair<bool, fix::Reject> reject = fix::Logon::Verify(_input.Message);

        if (reject.first) {
            Logger->log<logger::Level::Info>("Request verification failed");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger->log<logger::Level::Debug>("Reject moving to TCP output");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
            return false;
        } else if (_input.Client->isLoggedin()) {
            Logger->log<logger::Level::Warning>("Client (", *(_input.Client), ") already connected");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            reject.second.set58_text("Client already logged in");
            Logger->log<logger::Level::Debug>("Reject from (", *(_input.Client), ") moving to TCP output");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
            return false;
        }

        _input.Client->login(_input.Message.at(fix::Tag::SenderCompId));
        _input.Client->setSeqNumber(utils::to<size_t>(_input.Message.at(fix::Tag::MsqSeqNum)));

        Logger->log<logger::Level::Info>("Client set as logged in as: (", *(_input.Client), ")");
        logon.set98_EncryptMethod("0");
        logon.set108_HeartBtInt(_input.Message.at(fix::Tag::HearBtInt));
        Logger->log<logger::Level::Debug>("Reply to (", *(_input.Client), ") moving to TCP output");
        m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(logon));
        return true;
    }
}