#include <iomanip>
#include <chrono>
#include <format>

#include "Server/ProcessUnit/User/HeartBeat.hpp"

#include "Shared/Message-v2/Parser.hpp"
#include "Shared/Log/Manager.hpp"

namespace pu::user
{
    HeartBeatHandler::HeartBeatHandler(StringOutputQueue &_tcp_output)
        : AInputProcess<InputType>("Server/User/HeartBeat"),
        m_tcp_output(_tcp_output)
    {
        ClientStore::OnNewClient([this] (const ClientStore::Client &_client) {
            _client->getHeartBeatInfo().Since = std::chrono::system_clock::now();
        });

    }

    void HeartBeatHandler::onInput(InputType _input)
    {
        m_tp.enqueue([this, _input] () {
            switch (_input.Header.getPositional<fix42::tag::MsgType>().Value) {
                case fix42::msg::TestRequest::Type:
                    processTestRequest(_input);
                    break;
                case fix42::msg::HeartBeat::Type:
                    processHeartBeat(_input);
                    break;
            }
        });
    }

    void HeartBeatHandler::setup()
    {
        m_thread = std::jthread(&HeartBeatHandler::handle, this);
    }

    void HeartBeatHandler::onStop()
    {
        if (m_thread.joinable()) {
            Logger->log<logger::Level::Info>("Requesting stop on the worker thread");
            m_thread.request_stop();
            m_thread.join();
            Logger->log<logger::Level::Debug>("Worker thread joined");
        }
    }

    void HeartBeatHandler::processHeartBeat(const InputType &_input)
    {
        xstd::Expected<fix42::msg::HeartBeat, fix42::msg::SessionReject> error = fix42::parseMessage<fix42::msg::HeartBeat>(_input.Message, _input.Header);

        if (error.has_error()) {
            Logger->log<logger::Level::Info>("Parsing of HeartBeat message failed: ", error.error().get<fix42::tag::Text>().Value.value());
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(error.error().to_string()));
            return;
        }

        const fix42::msg::HeartBeat &heartbeat = error.value();
        InternalClient::HeartBeatInfo &hb = _input.Client->getHeartBeatInfo();

        if (hb.TestRequest) {
            if (hb.TestValue.has_value()) {
                if (!heartbeat.get<fix42::tag::TestReqId>().Value.has_value()) {
                    fix42::msg::SessionReject reject{};

                    reject.get<fix42::tag::RefTagId>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
                    reject.get<fix42::tag::RefTagId>().Value = fix42::msg::SessionReject::Type;
                    reject.get<fix42::tag::SessionRejectReason>().Value = fix42::RejectReasonSession::RequiredTagMissing;
                    reject.get<fix42::tag::Text>().Value = "Missing Test request Id";
                    Logger->log<logger::Level::Error>("Test request value missing for client: ", _input.Client->getUserId());
                    m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(reject.to_string()));
                    return;
                } else {
                    if (hb.TestValue.value() != heartbeat.get<fix42::tag::TestReqId>().Value.value()) {
                        fix42::msg::SessionReject reject{};

                        reject.get<fix42::tag::RefTagId>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
                        reject.get<fix42::tag::RefTagId>().Value = fix42::msg::SessionReject::Type;
                        reject.get<fix42::tag::SessionRejectReason>().Value = fix42::RejectReasonSession::ValueOutOfRange;
                        reject.get<fix42::tag::Text>().Value = "Incorrect Test Value";
                        Logger->log<logger::Level::Error>("Invalid Test Value: ", hb.TestValue.value(), " - ", heartbeat.get<fix42::tag::TestReqId>().Value.value(), ", for client: ", _input.Client->getUserId());
                        m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(reject.to_string()));
                        return;
                    }
                    hb.TestRequest = false;
                    hb.TestValue = std::nullopt;
                    Logger->log<logger::Level::Info>("Client (", _input.Client->getUserId(), ") TestRequest validated");
                }
            } else {
                Logger->log<logger::Level::Fatal>("Missing Test value in memory for client (", *(_input.Client), ")");
                return;
            }
        }

        fix42::msg::HeartBeat heartbeat_reply{};

        hb.Since = _input.ReceiveTime;
        Logger->log<logger::Level::Info>("Sending validation heartbeat to client: (", _input.Client->getUserId(), ")");
        m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::HeartBeat::Type, std::move(heartbeat_reply.to_string()));
    }

    void HeartBeatHandler::processTestRequest(const InputType &_input)
    {
        xstd::Expected<fix42::msg::TestRequest, fix42::msg::SessionReject> error = fix42::parseMessage<fix42::msg::TestRequest>(_input.Message, _input.Header);

        if (error.has_error()) {
            Logger->log<logger::Level::Info>("Parsing of TestRequest message failed: ", error.error().get<fix42::tag::Text>().Value.value());
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(error.error().to_string()));
            return;
        }

        const fix42::msg::TestRequest &testreq = error.value();
        fix42::msg::HeartBeat heartbeat{};

        heartbeat.get<fix42::tag::TestReqId>().Value = testreq.get<fix42::tag::TestReqId>().Value;
        Logger->log<logger::Level::Info>("Sending validation TestRequest (as HeartBeat) to client: (", _input.Client->getUserId(), ")");
        m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::HeartBeat::Type, std::move(heartbeat.to_string()));
    }

    void HeartBeatHandler::handle(std::stop_token _st)
    {
        Logger->log<logger::Level::Warning>("Entering processing thread");
        while (!_st.stop_requested()) {
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

            ClientStore::Instance().Apply([this, now] (ClientStore::Client _client) {
                InternalClient::HeartBeatInfo &hb_info = _client->getHeartBeatInfo();

                if (_client->shouldDisconnect() || !_client->isLoggedin())
                    return;
                if (now - hb_info.Since > std::chrono::seconds(hb_info.Elapsing)) {
                    if (!hb_info.TestRequest) {
                        fix42::msg::TestRequest testreq;

                        hb_info.Since = now;
                        hb_info.TestRequest = true;
                        hb_info.TestValue = std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::floor<std::chrono::seconds>(now));
                        testreq.get<fix42::tag::TestReqId>().Value = hb_info.TestValue.value();
                        Logger->log<logger::Level::Info>("Sending TestRequest to Client (", _client->getUserId(), ") for failed heartbeat");
                        m_tcp_output.append(_client, now, fix42::msg::TestRequest::Type, std::move(testreq.to_string()));
                    } else {
                        Logger->log<logger::Level::Error>("Client (", *(_client), ") failed heartbeat & Test Request");
                        ClientStore::Instance().removeClient(_client);
                    }
                }
            });
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        Logger->log<logger::Level::Warning>("Exiting processing thread");
    }
}