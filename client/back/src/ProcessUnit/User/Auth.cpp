#include "Client/Back/ProcessUnit/User/Auth.hpp"
#include "Client/Back/User.hpp"
#include "Client/Back/FrontManager.hpp"

#include "Client/Shared/IPC/Helper.hpp"
#include "Client/Shared/IPC/Message/Logon.hpp"

#include "Shared/Log/Manager.hpp"
#include "Shared/Message-v2/Parser.hpp"

namespace pu
{
    AuthHandler::AuthHandler(StringOutputQueue &_tcp_output)
        : AInputProcess<InputType>("Back/Auth"),
        m_tcp_output(_tcp_output)
    {
    }

    void AuthHandler::onInput(InputType _input)
    {
        switch (_input.Header.getPositional<fix42::tag::MsgType>().Value) {
            case fix42::msg::Logon::Type:
                handleLogon(_input);
                break;
            case fix42::msg::Logout::Type:
                handleLogout(_input);
                break;
            default:
                break;
        }
    }

    void AuthHandler::handleLogon(InputType &_input)
    {
        xstd::Expected<fix42::msg::Logon, fix42::msg::SessionReject> error = fix42::parseMessage<fix42::msg::Logon>(_input.Message, _input.Header);

        if (error.has_error()) {
            Logger->log<logger::Level::Info>("Parsing of Logon message failed: ", error.error().get<fix42::tag::Text>().Value.value());
            m_tcp_output.append(_input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(error.error().to_string()));
            return;
        }

        fix42::msg::Logon &logon = error.value();
        User &user = User::Instance();
        User::HeartBeatInfo &hb_info = user.getHeartBeatInfo();

        hb_info.Elapsing = logon.get<fix42::tag::HeartBtInt>().Value + 1;
        hb_info.Since = std::chrono::system_clock::now();
        Logger->log<logger::Level::Debug>("Using Elapsing as: ", hb_info.Elapsing);
        user.setSeqNumber(_input.Header.get<fix42::tag::MsgSeqNum>().Value + 1);
        user.login(_input.Header.get<fix42::tag::TargetCompId>().Value);

        FrontManager::Instance().send(ipc::Helper::Logon(ipc::msg::Logon{
            user.getUserId(),
            static_cast<uint32_t>(user.getSeqNumber()),
            hb_info.Elapsing
        }));
    }

    void AuthHandler::handleLogout(InputType &_input)
    {
        xstd::Expected<fix42::msg::Logout, fix42::msg::SessionReject> error = fix42::parseMessage<fix42::msg::Logout>(_input.Message, _input.Header);

        if (error.has_error()) {
            Logger->log<logger::Level::Info>("Parsing of Logon message failed: ", error.error().get<fix42::tag::Text>().Value.value());
            m_tcp_output.append(_input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(error.error().to_string()));
            return;
        }

        if (User::Instance().logoutRequested()) {
            Logger->log<logger::Level::Info>("Server accepted logout");
            // todo close server
        } else {
            Logger->log<logger::Level::Info>("Logout send from server");
            User::Instance().shouldDisconnect(true);
            m_tcp_output.append(_input.ReceiveTime, fix42::msg::Logout::Type, std::move(fix42::msg::Logout{}.to_string()));
        }
    }
}