#include "Server/ProcessUnit/User/Logon.hpp"
#include "Server/Config.hpp"

#include "Shared/Message-v2/Parser.hpp"
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
            xstd::Expected<fix42::msg::Logon, fix42::msg::SessionReject> error = fix42::parseMessage<fix42::msg::Logon>(_input.Message, _input.Header);

            if (error.has_error()) {
                Logger->log<logger::Level::Info>("Parsing of Logon message failed: ", error.error().get<fix42::tag::Text>().Value.value());
                m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(error.error().to_string()));
                return;
            }
            const fix42::msg::Logon &logon = error.value();

            if (!verifyMessage(logon, _input))
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
                hb_info.Elapsing = std::max(1.f, std::min(Configuration<config::Global>::Get().Config.User.Heartbeat.MaxTO, static_cast<float>(logon.get<fix42::tag::HeartBtInt>().Value)));

                Logger->log<logger::Level::Debug>("User (", _input.Client->getUserId(), ") use ", hb_info.Elapsing, "s as elapsing time between each HeartBeat");
                reply_logon.get<fix42::tag::EncryptMethod>().Value = fix42::EncryptionMethod::None;
                reply_logon.get<fix42::tag::HeartBtInt>().Value = hb_info.Elapsing;
                m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::Logon::Type, std::move(reply_logon.to_string()));
            }
        });
    }

    bool LogonHandler::verifyMessage(const fix42::msg::Logon &_logon, const InputType &_input)
    {
        if (_logon.get<fix42::tag::EncryptMethod>().Value != fix42::EncryptionMethod::None) {
            fix42::msg::SessionReject reject{};

            reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
            reject.get<fix42::tag::RefMsgType>().Value = _input.Header.getPositional<fix42::tag::MsgType>().Value;
            reject.get<fix42::tag::Text>().Value = "Encryption method not supported";
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(reject.to_string()));
            return false;
        }
        return true;
    }
}