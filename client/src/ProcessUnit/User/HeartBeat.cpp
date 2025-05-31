#include <chrono>

#include "Client/ProcessUnit/User/HeartBeat.hpp"
#include "Client/User.hpp"

#include "Common/Log/Manager.hpp"
#include "Common/Message/Message.hpp"
#include "Common/Message/Tag.hpp"

namespace pu
{
    HeartBeatHandler::HeartBeatHandler(QueueMessage &_tcp_output)
        : m_tcp_output(_tcp_output), Logger(log::Manager::newLogger("HeartBeat-handler"))
    {
        m_thread = std::jthread(&HeartBeatHandler::heartbeatLoop, this);
    }

    HeartBeatHandler::QueueInputType &HeartBeatHandler::getInput()
    {
        return m_input;
    }

    void HeartBeatHandler::runtime(std::stop_token _st)
    {
        InputType input;

        while (!_st.stop_requested()) {
            while (!m_input.empty()) {
                input = m_input.pop_front();
                switch (input.at("35")[0]) {
                    case fix::TestRequest::cMsgType: handleTestRequest(input);
                        break;
                    case fix::HeartBeat::cMsgType: handleHeartBeat(input);
                        break;
                }
            }
        }
    }

    void HeartBeatHandler::onStop()
    {
        if (m_thread.joinable()) {
            m_thread.request_stop();
            m_thread.join();
        }
    }

    bool HeartBeatHandler::handleTestRequest(InputType &_input)
    {
        fix::HeartBeat hb;
        std::pair<bool, fix::Reject> reject = fix::TestRequest::Verify(_input);

        if (reject.first) {
            if (reject.second.contains(fix::Tag::Text))
                Logger->log<log::Level::Info>("TestRequest | TestRequest verification failed: (", reject.second.get(fix::Tag::RefTagId), ") ", reject.second.get(fix::Tag::Text));
            else
                Logger->log<log::Level::Warning>("TestRequest | TestRequest verification failed for unknown reason");
            reject.second.set45_refSeqNum(_input.at(fix::Tag::MsqSeqNum));
            Logger->log<log::Level::Debug>("TestRequest | Reject moving to TCP output");
            m_tcp_output.append(std::move(reject.second));
            return false;
        }

        User::Instance().setSinceHeartBeat(std::chrono::system_clock::now());
        Logger->log<log::Level::Info>("TestRequest | TestRequest with Id: ", _input.at(fix::Tag::TestReqId));
        hb.set112_testReqID(_input.at(fix::Tag::TestReqId));
        m_tcp_output.append(std::move(hb));
        return true;
    }

    bool HeartBeatHandler::handleHeartBeat(InputType &_input)
    {
        std::pair<bool, fix::Reject> reject = fix::HeartBeat::Verify(_input);

        if (reject.first) {
            if (reject.second.contains(fix::Tag::Text))
                Logger->log<log::Level::Info>("HeartBeat | HeartBeat verification failed: (", reject.second.get(fix::Tag::RefTagId), ") ", reject.second.get(fix::Tag::Text));
            else
                Logger->log<log::Level::Warning>("HeartBeat | HeartBeat verification failed for unknown reason");
            reject.second.set45_refSeqNum(_input.at(fix::Tag::MsqSeqNum));
            Logger->log<log::Level::Debug>("HeartBeat | Reject moving to TCP output");
            m_tcp_output.append(std::move(reject.second));
            return false;
        }

        User::Instance().setSinceHeartBeat(_input.at(fix::Tag::SendingTime));
        Logger->log<log::Level::Info>("HeartBeat | HeartBeat hiting server correctly");
        return true;
    }

    void HeartBeatHandler::heartbeatLoop(std::stop_token _st)
    {
        while (!_st.stop_requested()) {
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

            if (User::Instance().isLogin() && std::chrono::duration<double>(now - User::Instance().getSinceHeartBeat()).count() > PU_HEARTBEAT_TO) {
                fix::HeartBeat hb;

                Logger->log<log::Level::Info>("Sending HeartBeat Message");
                User::Instance().setSinceHeartBeat(now);
                m_tcp_output.append(std::move(hb));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

}