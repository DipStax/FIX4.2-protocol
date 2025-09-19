#include <chrono>

#include "Client/Back/ProcessUnit/User/HeartBeat.hpp"
#include "Client/Back/User.hpp"

#include "Shared/Log/Manager.hpp"
#include "Shared/Message-v2/Parser.hpp"

namespace pu
{
    HeartBeatHandler::HeartBeatHandler(StringOutputQueue &_tcp_output)
        : AInputProcess<InputType>("Back/HeartBeat"),
        m_tcp_output(_tcp_output)
    {
        // Logger = logger::Manager::newLogger("file", getProcessName());

    }

    void HeartBeatHandler::onInput(InputType _input)
    {
        switch (_input.Header.getPositional<fix42::tag::MsgType>().Value) {
            case fix42::msg::TestRequest::Type:
                handleTestRequest(_input);
                break;
            case fix42::msg::HeartBeat::Type:
                handleHeartBeat(_input);
                break;
        }
    }

    void HeartBeatHandler::setup()
    {
        m_thread = std::jthread(&HeartBeatHandler::heartbeatLoop, this);
    }

    void HeartBeatHandler::onStop()
    {
        if (m_thread.joinable()) {
            m_thread.request_stop();
            m_thread.join();
        }
    }

    void HeartBeatHandler::handleTestRequest(const InputType &_input)
    {
        xstd::Expected<fix42::msg::TestRequest, fix42::msg::SessionReject> error = fix42::parseMessage<fix42::msg::TestRequest>(_input.Message, _input.Header);

        if (error.has_error()) {
            Logger->log<logger::Level::Info>("Parsing of TestRequest message failed: ", error.error().get<fix42::tag::Text>().Value.value());
            m_tcp_output.append(_input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(error.error().to_string()));
            return;
        }

        fix42::msg::HeartBeat hb;

        User::Instance().getHeartBeatInfo().Since = std::chrono::system_clock::now();
        Logger->log<logger::Level::Info>("TestRequest | TestRequest with Id: ", error.value().get<fix42::tag::TestReqId>().Value);
        hb.get<fix42::tag::TestReqId>().Value = error.value().get<fix42::tag::TestReqId>().Value;
        m_tcp_output.append(_input.ReceiveTime, fix42::msg::HeartBeat::Type, std::move(hb.to_string()));
    }

    void HeartBeatHandler::handleHeartBeat(const InputType &_input)
    {
        xstd::Expected<fix42::msg::HeartBeat, fix42::msg::SessionReject> error = fix42::parseMessage<fix42::msg::HeartBeat>(_input.Message, _input.Header);

        if (error.has_error()) {
            Logger->log<logger::Level::Info>("Parsing of HeartBeat message failed: ", error.error().get<fix42::tag::Text>().Value.value());
            m_tcp_output.append(_input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(error.error().to_string()));
            return;
        }
        User::Instance().getHeartBeatInfo().Since = _input.Header.get<fix42::tag::SendingTime>().Value;
        Logger->log<logger::Level::Info>("HeartBeat | HeartBeat hiting server correctly");
    }

    void HeartBeatHandler::heartbeatLoop(std::stop_token _st)
    {
        while (!User::Instance().isLogin()) {}

        User::HeartBeatInfo &hb_info = User::Instance().getHeartBeatInfo();
        Logger->log<logger::Level::Verbose>("Using elapsing time as: ", hb_info.Elapsing);

        while (!_st.stop_requested()) {
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

            if (User::Instance().isLogin() && now - hb_info.Since > std::chrono::seconds(hb_info.Elapsing)) {
                Logger->log<logger::Level::Info>("Sending HeartBeat Message");
                hb_info.Since = now;
                m_tcp_output.append(std::chrono::system_clock::now(), fix42::msg::HeartBeat::Type, std::move(fix42::msg::HeartBeat{}.to_string()));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}