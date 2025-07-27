#include <iomanip>
#include <chrono>
#include <format>

#include "Server/Core/ProcessUnit/User/HeartBeat.hpp"

#include "Common/Message/HeartBeat.hpp"
#include "Common/Message/Tag.hpp"
#include "Common/Log/Manager.hpp"

namespace pu::user
{
    HeartBeatHandler::HeartBeatHandler(InputNetworkOutput &_tcp_output)
        : AInputProcess<InputType>("Server/User/HeartBeat"),
        m_tcp_output(_tcp_output)
    {
        Logger = logger::Manager::newLogger("file", "Server/User/HeartBeat");

        ClientStore::OnNewClient([this] (const ClientStore::Client &_client) {
            _client->getHeartBeatInfo().Since = std::chrono::system_clock::now();
        });

        m_thread = std::jthread(&HeartBeatHandler::handle, this);
    }

    void HeartBeatHandler::onInput(InputType _input)
    {
        m_tp.enqueue([this, _input] () {
            switch (_input.Message.at("35")[0]) {
                case fix::TestRequest::cMsgType:
                    break;
                case fix::HeartBeat::cMsgType:
                    processHeartBeat(_input);
                    break;
                default:
                    Logger->log<logger::Level::Error>("Unknow message", _input.Message.at("35")[0]);
                    break;
            }
        });
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

    bool HeartBeatHandler::processHeartBeat(const InputType &_input)
    {
        Logger->log<logger::Level::Info>("Processing message...");
        fix::HeartBeat heartbeat;
        std::pair<bool, fix::Reject> reject = fix::HeartBeat::Verify(_input.Message);

        if (reject.first) {
            if (reject.second.contains(fix::Tag::Text))
                Logger->log<logger::Level::Info>("Header verification failed: (", reject.second.get(fix::Tag::RefTagId), ") ", reject.second.get(fix::Tag::Text));
            else
                Logger->log<logger::Level::Warning>("Header verification failed for unknown reason");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger->log<logger::Level::Debug>("Reject from (", *(_input.Client), ") moving to TCP output");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
            return false;
        }

        InternalClient::HeartBeatInfo &hb = _input.Client->getHeartBeatInfo();

        Logger->log<logger::Level::Info>("Updated client (", *(_input.Client), ") heartbeat");
        if (hb.TestRequest) {
            if (hb.TestValue.has_value()) {
                if (hb.TestValue.value() != _input.Message.at(fix::Tag::TestReqId)) {
                    fix::Reject reject;

                    reject.set371_refTagId(_input.Message.at(fix::Tag::MsqSeqNum));
                    reject.set372_refMsgType(_input.Message.at(fix::Tag::MsgType));
                    reject.set373_sessionRejectReason(fix::Reject::ValueOORange);
                    reject.set58_text("Incorrect Test Value");
                    Logger->log<logger::Level::Error>("Invalid Test Value: ", hb.TestValue.value(), " - ", _input.Message[fix::Tag::TestReqId], ", for client: (", *(_input.Client), ")");
                    m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject));
                    return false;
                }
                hb.TestRequest = false;
                hb.TestValue = std::nullopt;
            } else {
                Logger->log<logger::Level::Fatal>("Missing Test value in memory for client (", *(_input.Client), ")");
                return false;
            }
        }
        hb.Since = _input.ReceiveTime;
        Logger->log<logger::Level::Debug>("Heartbeat from (", *(_input.Client), ") moving to TCP output");
        m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(heartbeat));
        return true;
    }

    void HeartBeatHandler::handle(std::stop_token _st)
    {
        while (!_st.stop_requested()) {
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

            ClientStore::Instance().Apply([this, now] (ClientStore::Client _client) {
                InternalClient::HeartBeatInfo &hb_info = _client->getHeartBeatInfo();

                if (_client->shouldDisconnect() || !_client->isLoggedin())
                    return;
                if (std::chrono::duration<double>(now - hb_info.Since).count() > hb_info.Elapsing) {
                    if (!hb_info.TestRequest) {
                        fix::TestRequest test;

                        hb_info.Since = now;
                        hb_info.TestRequest = true;
                        hb_info.TestValue = std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::floor<std::chrono::seconds>(now));
                        test.set112_testReqID(hb_info.TestValue.value());
                        Logger->log<logger::Level::Info>("Sending TestRequest to Client (", *(_client), ") for failed heartbeat");
                        m_tcp_output.append(_client, now, std::move(test));
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