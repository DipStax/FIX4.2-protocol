#include "Server/ProcessUnit/User/Logout.hpp"

#include "Shared/Message-v2/Parser.hpp"
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
            xstd::Expected<fix42::msg::Logout, fix42::msg::SessionReject> error = fix42::parseMessage<fix42::msg::Logout>(_input.Message, _input.Header);

            if (error.has_error()) {
                Logger->log<logger::Level::Info>("Parsing of Logout message failed: ", error.error().get<fix42::tag::Text>().Value.value());
                m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(error.error().to_string()));
                return;
            }
            fix42::msg::Logout reply_logout{};

            _input.Client->shouldDisconnect(true);
            Logger->log<logger::Level::Info>("Client (", _input.Client->getUserId(), ") set as logged out");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::Logout::Type, std::move(reply_logout.to_string()));
        });
    }
}