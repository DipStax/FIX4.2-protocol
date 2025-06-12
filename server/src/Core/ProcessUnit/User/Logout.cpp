#include "Server/Core/ProcessUnit/User/Logout.hpp"

#include "Common/Utils.hpp"
#include "Common/Message/Logon.hpp"
#include "Common/Message/Tag.hpp"
#include "Common/Log/Manager.hpp"

namespace pu::user
{
    LogoutHandler::LogoutHandler(InputNetworkOutput &_tcp_output)
        : m_tcp_output(_tcp_output), Logger(logger::Manager::newLogger("Logout"))
    {
    }

    LogoutHandler::QueueInputType &LogoutHandler::getInput()
    {
        return m_input;
    }

    void LogoutHandler::runtime(std::stop_token _st)
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

    bool LogoutHandler::process(InputType &&_input)
    {
        Logger->log<logger::Level::Info>("Processing message...");
        fix::Logout logout;
        std::pair<bool, fix::Reject> reject = fix::Logout::Verify(_input.Message);

        if (reject.first) {
            Logger->log<logger::Level::Info>("Request verification failed");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger->log<logger::Level::Debug>("Reject from (", *(_input.Client), ") moving to TCP output");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
            return false;
        } else if (!_input.Client->isLoggedin()) {
            Logger->log<logger::Level::Info>("Client try to logout without begin connected");
            reject.second.set58_text("Client not connected");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger->log<logger::Level::Debug>("(Logon) Reject moving to TCP output");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
            return false;
        }

        _input.Client->shouldDisconnect(true);

        Logger->log<logger::Level::Info>("Client set as logged out: (", *(_input.Client), ")");
        Logger->log<logger::Level::Debug>("Reply to (", *(_input.Client), ") moving to TCP output");
        m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(logout));
        return true;
    }
}