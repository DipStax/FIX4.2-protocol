#include "Client/Back/ProcessUnit/Router.hpp"
#include "Client/Back/User.hpp"

#include "Common/Log/Manager.hpp"
#include "Common/Message/Message.hpp"
#include "Common/Message/Tag.hpp"

namespace pu
{
    Router::Router(QueueMessage &_tcp_output, QueueTransit &_heartbeat, QueueTransit &_auth)
        : m_tcp_output(_tcp_output), m_heartbeat(_heartbeat), m_auth(_auth),
        Logger(log::Manager::newLogger("Router"))
    {
    }

    QueueTransit &Router::getInput()
    {
        return m_input;
    }

    void Router::runtime(std::stop_token _st)
    {
        Logger->log<log::Level::Info>("Starting process unit...");
        TransitMessage input;
        std::pair<bool, fix::Reject> reject;

        while (!_st.stop_requested()) {
            while (!m_input.empty()) {
                input = m_input.pop_front();
                reject = fix::Header::Verify(input, PROVIDER_NAME, User::Instance().getUserId(), User::Instance().getSeqNumber());
                if (reject.first) {
                    if (reject.second.contains(fix::Tag::Text))
                        Logger->log<log::Level::Info>("Header verification failed: (", reject.second.get(fix::Tag::RefTagId), ") ", reject.second.get(fix::Tag::Text));
                    else
                        Logger->log<log::Level::Warning>("Header verification failed for unknown reason");
                    // m_tcp_output.append(std::move(reject.second));
                    continue;
                }
                Logger->log<log::Level::Debug>("Header verification validated");

                switch (input.at(fix::Tag::MsgType)[0])
                {
                    // case fix::Logon::cMsgType:
                    // case fix::Logout::cMsgType:
                    //     m_
                    case fix::HeartBeat::cMsgType:
                    case fix::TestRequest::cMsgType:
                        m_heartbeat.push(std::move(input));
                        break;
                    default:
                        unknownMessage(input);
                };
            }
        }
    }

    bool Router::unknownMessage(InputType &_input)
    {
        fix::Reject reject;

        Logger->log<log::Level::Info>("Rejecting request from server with request type: ", _input.at(fix::Tag::MsgType));
        reject.set45_refSeqNum(_input.at(fix::Tag::MsqSeqNum));
        reject.set371_refTagId(fix::Tag::MsgType);
        reject.set373_sessionRejectReason(fix::Reject::NotSupporType);
        reject.set58_text("Unknown message type");
        Logger->log<log::Level::Debug>("Moving Reject Unknown from server to TCP Output");
        // m_tcp_output.append(std::move(reject));
        return true;
    }
}