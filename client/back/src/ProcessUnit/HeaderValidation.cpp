#include "Client/Back/ProcessUnit/HeaderValidation.hpp"

#include "Client/Back/User.hpp"
#include "Client/Back/Config.hpp"

namespace pu
{
    HeaderValidation::HeaderValidation(UnparsedMessageQueue &_output, StringOutputQueue &_error)
        : AInputProcess<InputType>("Back/Header-verification"),
        m_output(_output), m_error(_error)
    {
    }

    void HeaderValidation::onInput(InputType _input)
    {
        std::optional<fix42::msg::SessionReject> reject = verifyPositionValue(_input.Header);

        if (reject.has_value()) {
            Logger->log<logger::Level::Error>("Positional tag verification failed: ", reject.value().get<fix42::tag::Text>().Value.value());
            m_error.append(_input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(reject.value().to_string()));
            return;
        }
        Logger->log<logger::Level::Info>("Validated positional tag from message");
        // todo handle secure data
        reject = verifyUserSpecific(_input.Header);
        if (reject.has_value()) {
            Logger->log<logger::Level::Error>("User specific verification failed: ", reject.value().get<fix42::tag::Text>().Value.value());
            m_error.append(_input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(reject.value().to_string()));
            return;
        }
        // todo verify time accuracy
        Logger->log<logger::Level::Info>("Validated user specific tag from message");
        m_output.push(std::move(_input));
    }

    std::optional<fix42::msg::SessionReject> HeaderValidation::verifyPositionValue(const fix42::Header &_header)
    {
        fix42::msg::SessionReject reject{};

        reject.get<fix42::tag::RefSeqNum>().Value = _header.get<fix42::tag::MsgSeqNum>().Value;
        reject.get<fix42::tag::RefMsgType>().Value = _header.getPositional<fix42::tag::MsgType>().Value;
        if (_header.getPositional<fix42::tag::BeginString>().Value != "FIX.4.2") {
            reject.get<fix42::tag::SessionRejectReason>().Value = fix42::RejectReasonSession::ValueOutOfRange;
            reject.get<fix42::tag::RefTagId>().Value = fix42::tag::BeginString;
            reject.get<fix42::tag::Text>().Value = "Incorrect begin string. Expected FIX.4.2";
            return reject;
        }
        switch (_header.getPositional<fix42::tag::MsgType>().Value) {
            case fix42::msg::HeartBeat::Type:
            case fix42::msg::TestRequest::Type:
            case fix42::msg::SessionReject::Type:
            case fix42::msg::Logout::Type:
            case fix42::msg::ExecutionReport::Type:
            case fix42::msg::BusinessReject::Type:
            case fix42::msg::Logon::Type:
                return std::nullopt;
            default:
                reject.get<fix42::tag::SessionRejectReason>().Value = fix42::RejectReasonSession::InvalidMsgType;
                reject.get<fix42::tag::RefTagId>().Value = fix42::tag::MsgType;
                reject.get<fix42::tag::Text>().Value = "Not supported message type";
                return reject;
        }
    }

    std::optional<fix42::msg::SessionReject> HeaderValidation::verifyUserSpecific(const fix42::Header &_header)
    {
        fix42::msg::SessionReject reject{};

        reject.get<fix42::tag::RefSeqNum>().Value = _header.get<fix42::tag::MsgSeqNum>().Value;
        reject.get<fix42::tag::RefMsgType>().Value = _header.getPositional<fix42::tag::MsgType>().Value;
        if (_header.get<fix42::tag::SenderCompId>().Value != Configuration<config::Global>::Get().Config.FixServer.ProviderName) {
            reject.get<fix42::tag::SessionRejectReason>().Value = fix42::RejectReasonSession::ValueOutOfRange;
            reject.get<fix42::tag::RefTagId>().Value = fix42::tag::TargetCompId;
            reject.get<fix42::tag::Text>().Value = "Incorrect target Id";
            return reject;
        }
        if (User::Instance().isLogin()) {
            if (_header.get<fix42::tag::TargetCompId>().Value != User::Instance().getUserId()) {
                reject.get<fix42::tag::SessionRejectReason>().Value = fix42::RejectReasonSession::ValueOutOfRange;
                reject.get<fix42::tag::RefTagId>().Value = fix42::tag::SenderCompId;
                reject.get<fix42::tag::Text>().Value = "Incorrect sender Id";
                return reject;
            } else if (_header.get<fix42::tag::MsgSeqNum>().Value != User::Instance().getSeqNumber() - 1) {
                reject.get<fix42::tag::SessionRejectReason>().Value = fix42::RejectReasonSession::ValueOutOfRange;
                reject.get<fix42::tag::RefTagId>().Value = fix42::tag::MsgSeqNum;
                reject.get<fix42::tag::Text>().Value = "Inccorect message sequence number";
                return reject;
            }
        }
        return std::nullopt;
    }
}