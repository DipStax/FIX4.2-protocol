#include "Server/Core/ProcessUnit/User/Logon.hpp"

#include "Common/Core/Utils.hpp"
#include "Common/Message/Logon.hpp"
#include "Common/Message/Tag.hpp"

namespace pu::user
{
    LogonHandler::LogonHandler(InOutNetwork &_tcp_output)
        : m_tcp_output(_tcp_output)
    {
    }

    LogonHandler::QueueInputType &LogonHandler::getInput()
    {
        return m_input;
    }

    std::string LogonHandler::getThreadName() const
    {
        return "Logon Handler";
    }

    void LogonHandler::runtime(std::stop_token _st)
    {
        while (!_st.stop_requested()) {
            while (!m_input.empty()) {
                m_tp.enqueue([this, _input = std::move(m_input.pop_front())] () mutable {
                    process(std::move(_input));
                });
            }
        }
    }

    bool LogonHandler::process(InputType &&_input)
    {
        Logger::Log("Processing message...");
        fix::Logon logon;
        std::pair<bool, fix::Reject> reject = fix::Logon::Verify(_input.Message);

        if (reject.first) {
            Logger::Log("Request verification failed");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger::Log("Reject moving to TCP output");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
            return false;
        } else if (_input.Client->isLoggedin()) {
            Logger::Log("Client (", *(_input.Client), ") already connected");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            reject.second.set58_text("Client already logged in");
            Logger::Log("Reject from (", *(_input.Client), ") moving to TCP output");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
            return false;
        }

        _input.Client->login(_input.Message.at(fix::Tag::SenderCompId));
        _input.Client->setSeqNumber(utils::to<size_t>(_input.Message.at(fix::Tag::MsqSeqNum)));

        Logger::Log("Client set as logged in as: (", *(_input.Client), ")");
        logon.set98_EncryptMethod("0");
        logon.set108_HeartBtInt(_input.Message.at(fix::Tag::HearBtInt));
        Logger::Log("Reply to (", *(_input.Client), ") moving to TCP output");
        m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(logon));
        return true;
    }
}