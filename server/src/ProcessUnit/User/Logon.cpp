#include "Server/ProcessUnit/User/Logon.hpp"
#include "Server/Config.hpp"

#include "Shared/Log/Manager.hpp"

namespace pu::user
{
    LogonHandler::LogonHandler(StringOutputQueue &_tcp_output)
        : AInputProcess<InputType>("Server/User/Logon"),
        m_tcp_output(_tcp_output)
    {
    }

    void LogonHandler::onInput(InputType _input)
    {
        m_tp.enqueue([this, _input = std::move(_input)] () {
            std::optional<fix42::msg::Logon> logon = parseMessage(_input);

            if (!logon.has_value() && !verifyMessage(logon.value(), _input))
                return;
            if (_input.Client->isLoggedin()) {
                fix42::msg::SessionReject reject{};

                reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
                reject.get<fix42::tag::RefMsgType>().Value = _input.Header.getPositional<fix42::tag::MsgType>().Value;
                reject.get<fix42::tag::SessionRejectReason>().Value = fix42::RejectReasonSession::InvalidMsgType;
                reject.get<fix42::tag::Text>().Value = "User is already logged in";
                Logger->log<logger::Level::Warning>("User (", _input.Client->getUserId(), ") already connect, but trying to reconnect");
                m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(reject.to_string()));
            } else {
                fix42::msg::Logon reply_logon{};
                InternalClient::HeartBeatInfo &hb_info = _input.Client->getHeartBeatInfo();

                _input.Client->login(_input.Header.get<fix42::tag::SenderCompId>().Value);
                _input.Client->setSeqNumber(_input.Header.get<fix42::tag::MsgSeqNum>().Value + 1);
                Logger->log<logger::Level::Info>("User logged in: ", _input.Client->getUserId());

                hb_info.Since = std::chrono::system_clock::now();
                hb_info.Elapsing = std::max(1.f, std::min(Configuration<config::Global>::Get().Config.User.Heartbeat.MaxTO, static_cast<float>(logon.value().get<fix42::tag::HeartBtInt>().Value)));

                Logger->log<logger::Level::Debug>("User (", _input.Client->getUserId(), ") use ", hb_info.Elapsing, "s as elapsing time between each HeartBeat");
                reply_logon.get<fix42::tag::EncryptMethod>().Value = fix42::EncryptionMethod::None;
                reply_logon.get<fix42::tag::HeartBtInt>().Value = hb_info.Elapsing;
                m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::Logon::Type, std::move(reply_logon.to_string()));
            }
        });
    }

    std::optional<fix42::msg::Logon> LogonHandler::parseMessage(InputType _input)
    {
        fix42::msg::Logon logon{};
        std::optional<fix::RejectError> error = logon.from_string(_input.Message);

        if (error.has_value()) {
            fix42::msg::SessionReject reject{};

            reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
            reject.get<fix42::tag::RefMsgType>().Value = fix42::msg::Logon::Type;
            reject.get<fix42::tag::SessionRejectReason>().Value = static_cast<fix42::RejectReasonSession>(error.value().Reason);
            reject.get<fix42::tag::RefTagId>().Value = error.value().Tag;
            reject.get<fix42::tag::Text>().Value = error.value().Message;
            Logger->log<logger::Level::Info>("Parsing Logon message failed, reason: ", reject.get<fix42::tag::Text>().Value.value());
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(reject.to_string()));
            return std::nullopt;
        }
        return logon;
    }

    bool LogonHandler::verifyMessage(const fix42::msg::Logon &_logon, const InputType &_input)
    {
        if (_logon.get<fix42::tag::EncryptMethod>().Value != fix42::EncryptionMethod::None) {
            fix42::msg::SessionReject reject{};

            reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
            reject.get<fix42::tag::RefMsgType>().Value = _input.Header.getPositional<fix42::tag::MsgType>().Value;
            reject.get<fix42::tag::SessionRejectReason>().Value = fix42::RejectReasonSession::InvalidMsgType;
            reject.get<fix42::tag::Text>().Value = "Encryption method not supported";
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(reject.to_string()));
            return false;
        }
        return true;
    }
}