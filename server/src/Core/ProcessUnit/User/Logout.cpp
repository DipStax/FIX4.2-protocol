#include "Server/Core/ProcessUnit/User/Logout.hpp"

#include "Common/Core/Utils.hpp"
#include "Common/Message/Logon.hpp"
#include "Common/Message/Tag.hpp"

namespace pu::user
{
    LogoutHandler::LogoutHandler(InputNetworkOutput &_tcp_output)
        : m_tcp_output(_tcp_output)
    {
    }

    LogoutHandler::QueueInputType &LogoutHandler::getInput()
    {
        return m_input;
    }

    std::string LogoutHandler::getThreadName() const
    {
        return "Logout Handler";
    }

    void LogoutHandler::runtime(std::stop_token _st)
    {
        while (!_st.stop_requested()) {
            while (!m_input.empty()) {
                m_tp.enqueue([this, _input = std::move(m_input.pop_front())] () mutable {
                    process(std::move(_input));
                });
            }
        }
    }

    bool LogoutHandler::process(InputType &&_input)
    {
        Logger::Log("Processing message...");
        fix::Logout logout;
        std::pair<bool, fix::Reject> reject = fix::Logout::Verify(_input.Message);

        if (reject.first) {
            Logger::Log("Request verification failed");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger::Log("Reject from (", *(_input.Client), ") moving to TCP output");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
            return false;
        } else if (!_input.Client->isLoggedin()) {
            Logger::Log("Client try to logout without begin connected");
            reject.second.set58_text("Client not connected");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger::Log("(Logon) Reject moving to TCP output");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
            return false;
        }

        _input.Client->shouldDisconnect(true);

        Logger::Log("Client set as logged out: (", *(_input.Client), ")");
        Logger::Log("Reply to (", *(_input.Client), ") moving to TCP output");
        m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(logout));
        return true;
    }
}