#include "Client/Back/ProcessUnit/User/Auth.hpp"
#include "Client/Back/User.hpp"
#include "Client/Back/FrontManager.hpp"

#include "Client/Common/IPC/Helper.hpp"
#include "Client/Common/IPC/Message/Logon.hpp"

#include "Common/Message/Tag.hpp"
#include "Common/Log/Manager.hpp"

namespace pu
{
    AuthHandler::AuthHandler(QueueMessage &_tcp_output)
        : m_tcp_output(_tcp_output), Logger(logger::Manager::newLogger("Auth"))
    {
    }

    AuthHandler::QueueInputType &AuthHandler::getInput()
    {
        return m_input;
    }

    void AuthHandler::runtime(std::stop_token _st)
    {
        Logger->log<logger::Level::Info>("Starting process unit...");
        InputType input;

        while (!_st.stop_requested()) {
            while (!m_input.empty()) {
                input = m_input.pop_front();

                switch (input.at(fix::Tag::MsgType)[0]) {
                case fix::Logon::cMsgType: handleLogon(input);
                    break;
                case fix::Logout::cMsgType: handleLogout(input);
                    break;
                default:
                    break;
                }
            }
        }
    }

    bool AuthHandler::handleLogon(InputType &_input)
    {
        std::pair<bool, fix::Reject> reject = fix::Logon::Verify(_input);

        if (reject.first) {
            if (reject.second.contains(fix::Tag::Text))
                Logger->log<logger::Level::Info>("Logon | Logon verification failed: (", reject.second.get(fix::Tag::RefTagId), ") ", reject.second.get(fix::Tag::Text));
            else
                Logger->log<logger::Level::Warning>("Logon | Logon verification failed for unknown reason");
            reject.second.set45_refSeqNum(_input.at(fix::Tag::MsqSeqNum));
            Logger->log<logger::Level::Debug>("Logon | Reject moving to TCP output");
            m_tcp_output.append(std::move(reject.second));
            return false;
        }

        User &user = User::Instance();
        User::HeartBeatInfo &hb_info = user.getHeartBeatInfo();
        ipc::msg::Logon ipc_logon{};

        hb_info.Elapsing = utils::to<float>(_input.at(fix::Tag::HeartBtInt));
        Logger->log<logger::Level::Debug>("Using Elapsing as: ", hb_info.Elapsing);
        user.login(_input.at(fix::Tag::TargetCompId));

        ipc_logon.UserId = user.getUserId();
        ipc_logon.SeqNum = static_cast<uint32_t>(user.getSeqNumber());
        ipc_logon.HeartBeat = hb_info.Elapsing;
        FrontManager::Instance().notify(ipc::Helper::Logon(ipc_logon));
        return true;
    }

    bool AuthHandler::handleLogout(InputType &_input)
    {
        std::pair<bool, fix::Reject> reject = fix::Logout::Verify(_input);

        if (reject.first) {
            if (reject.second.contains(fix::Tag::Text))
                Logger->log<logger::Level::Info>("Logout | Logout verification failed: (", reject.second.get(fix::Tag::RefTagId), ") ", reject.second.get(fix::Tag::Text));
            else
                Logger->log<logger::Level::Warning>("Logout | Logout verification failed for unknown reason");
            reject.second.set45_refSeqNum(_input.at(fix::Tag::MsqSeqNum));
            Logger->log<logger::Level::Debug>("Logout | Reject moving to TCP output");
            m_tcp_output.append(std::move(reject.second));
            return false;
        }

        if (User::Instance().logoutRequested()) {
            Logger->log<logger::Level::Info>("Server accepted logout");
            // close server
        } else {
            Logger->log<logger::Level::Info>("Logout send from server");
            User::Instance().shouldDisconnect(true);
            Logger->log<logger::Level::Debug>("Reply with client logout movingto TCP output");
            m_tcp_output.push(std::move(fix::Logout{}));
        }
        return true;
    }
}