#include "Client/ProcessUnit/Router.hpp"
#include "Client/User.hpp"

#include "Common/Log/Manager.hpp"
#include "Common/Message/Message.hpp"

namespace pu
{
    Router::Router(QueueTransit &_heartbeat)
        : m_heartbeat(_heartbeat), Logger(log::Manager::newLogger("Router"))
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
                reject = fix::Header::Verify(input, User::Instance().getName(), PROVIDER_NAME, User::Instance().getSeqNumber());
                if (reject.first) {
                    Logger->log<log::Level::Info>("Header verification failed");
                    // m_tcp_output.append(std::move(reject.second));
                    continue;
                }
                Logger->log<log::Level::Debug>("Header verification validated");
                switch (input.at("35")[0])
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
        Logger->log<log::Level::Info>("(Unknown) Processing message...");
        fix::Reject reject;

        Logger->log<log::Level::Info>("(New Order Single) Rejecting request from server with request type: ", _input.at(fix::Tag::MsgType));
        reject.set45_refSeqNum(_input.at(fix::Tag::MsqSeqNum));
        reject.set371_refTagId(fix::Tag::MsgType);
        reject.set373_sessionRejectReason(fix::Reject::NotSupporType);
        reject.set58_text("Unknown message type");
        Logger->log<log::Level::Debug>("(Unknown) Moving Reject Unknown from server to TCP Output");
        // m_tcp_output.append(std::move(reject));
        return true;
    }
}