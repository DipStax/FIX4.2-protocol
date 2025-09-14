#include "Server/ProcessUnit/User/Logout.hpp"

#include "Shared/Log/Manager.hpp"

namespace pu::user
{
    LogoutHandler::LogoutHandler(StringOutputQueue &_tcp_output)
        : AInputProcess<InputType>("Server/User/Logout"),
        m_tcp_output(_tcp_output)
    {
    }

    void LogoutHandler::onInput(InputType _input)
    {
        m_tp.enqueue([this, _input = std::move(_input)] () mutable {
            std::optional<fix42::msg::Logout> logout = parseMessage(_input);

            if (!logout.has_value())
                return;
            fix42::msg::Logout reply_logout{};

            _input.Client->shouldDisconnect(true);
            Logger->log<logger::Level::Info>("Client (", _input.Client->getUserId(), ") set as logged out");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::Logout::Type, std::move(reply_logout.to_string()));
        });
    }

    std::optional<fix42::msg::Logout> LogoutHandler::parseMessage(InputType _input)
    {
        fix42::msg::Logout logout{};
        std::optional<fix::RejectError> error = logout.from_string(_input.Message);

        if (error.has_value()) {
            fix42::msg::SessionReject reject{};

            reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
            reject.get<fix42::tag::RefMsgType>().Value = fix42::msg::Logout::Type;
            reject.get<fix42::tag::SessionRejectReason>().Value = static_cast<fix42::RejectReasonSession>(error.value().Reason);
            reject.get<fix42::tag::RefTagId>().Value = error.value().Tag;
            reject.get<fix42::tag::Text>().Value = error.value().Message;
            Logger->log<logger::Level::Info>("Parsing Logout message failed, reason: ", reject.get<fix42::tag::Text>().Value.value());
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(reject.to_string()));
            return std::nullopt;
        }
        return logout;
    }
}