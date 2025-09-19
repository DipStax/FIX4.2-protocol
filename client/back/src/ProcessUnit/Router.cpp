#include "Client/Back/ProcessUnit/Router.hpp"
#include "Client/Back/User.hpp"
#include "Client/Back/Config.hpp"

#include "Shared/Log/Manager.hpp"

namespace pu
{
    Router::Router(UnparsedMessageQueue &_heartbeat, UnparsedMessageQueue &_auth, UnparsedMessageQueue &_exec, StringOutputQueue &_tcp_output)
        : AInputProcess<InputType>("Back/Master-Router"),
        m_heartbeat(_heartbeat), m_auth(_auth), m_execution(_exec), m_error(_tcp_output)
    {
    }

    void Router::onInput(InputType _input)
    {
        switch (_input.Header.getPositional<fix42::tag::MsgType>().Value) {
            case fix42::msg::Logon::Type:
            case fix42::msg::Logout::Type:
                m_auth.push(std::move(_input));
                break;
            case fix42::msg::HeartBeat::Type:
            case fix42::msg::TestRequest::Type:
                m_heartbeat.push(std::move(_input));
                break;
            case fix42::msg::ExecutionReport::Type:
                m_execution.push(std::move(_input));
                break;
            case fix42::msg::BusinessReject::Type:
                treatBusinessReject(_input);
                break;
            case fix42::msg::SessionReject::Type:
                treatReject(_input);
                break;
            default:
                unknownMessage(_input);
        };
    }

    void Router::unknownMessage(const InputType &_input)
    {
        fix42::msg::SessionReject reject{};

        Logger->log<logger::Level::Info>("Rejecting request from server with request type: ", _input.Header.get<fix42::tag::MsgSeqNum>().Value);
        reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
        reject.get<fix42::tag::RefTagId>().Value = fix42::tag::MsgType;
        reject.get<fix42::tag::RefMsgType>().Value = _input.Header.getPositional<fix42::tag::MsgType>().Value;
        reject.get<fix42::tag::SessionRejectReason>().Value = fix42::RejectReasonSession::InvalidMsgType;
        reject.get<fix42::tag::Text>().Value = "Unknown message type";
        m_error.append(_input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(reject.to_string()));
    }

    void Router::treatReject(const InputType &_input)
    {
        // todo log
        // Logger->log<logger::Level::Error>("Rejected message: { refSeqNum: ", _input.at(fix::Tag::RefSeqNum), ", refTagId: ", _input.at(fix::Tag::RefTagId), ", reason: ", _input.at(fix::Tag::SessionRejectReason), ", text: ", _input.at(fix::Tag::Text)," }");
    }

    void Router::treatBusinessReject(InputType &_input)
    {
        // todo log
        // Logger->log<logger::Level::Error>("Reject Business message: { refSeqNum: ", _input.at(fix::Tag::RefSeqNum), ", reject refId: ", _input.at(fix::Tag::BusinessRejectRefId), ", reason", _input.at(fix::Tag::BusinessRejectReason),", text: ", _input.at(fix::Tag::Text));
    }
}