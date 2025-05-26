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
        : m_tcp_output(_tcp_output), Logger(log::Manager::newLogger("HeartBeat"))
    {
        ClientStore::OnNewClient([this] (const ClientStore::Client &_client) {
            _client->getHeartBeatInfo().Since = std::chrono::system_clock::now();
        });

        m_thread = std::jthread(&HeartBeatHandler::handle, this);
    }

    HeartBeatHandler::QueueInputType &HeartBeatHandler::getInput()
    {
        return m_input;
    }

    void HeartBeatHandler::runtime(std::stop_token _st)
    {
        Logger->log<log::Level::Info>("Starting process unit...");

        while (!_st.stop_requested()) {
            while (!m_input.empty()) {
                m_tp.enqueue([this, _input = std::move(m_input.pop_front())] () {
                    switch (_input.Message.at("35")[0]) {
                        case fix::TestRequest::cMsgType:
                            break;
                        case fix::HeartBeat::cMsgType:
                            processHeartBeat(_input);
                            break;
                    }
                    Logger->log<log::Level::Error>("Unknow message", _input.Message.at("35")[0]);
                });
            }
        }
        Logger->log<log::Level::Warning>("Exiting process unit...");
    }

    void HeartBeatHandler::onStop()
    {
        if (m_thread.joinable()) {
            m_thread.request_stop();
            m_thread.join();
        }
    }

    bool HeartBeatHandler::processHeartBeat(const InputType &_input)
    {
        Logger->log<log::Level::Info>("Processing message...");
        fix::HeartBeat heartbeat;
        std::pair<bool, fix::Reject> reject = fix::HeartBeat::Verify(_input.Message);

        if (reject.first) {
            Logger->log<log::Level::Info>("Request verification failed: ");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger->log<log::Level::Debug>("Reject from (", *(_input.Client), ") moving to TCP output");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
            return false;
        }

        InternalClient::HeartBeatInfo &hb = _input.Client->getHeartBeatInfo();

        Logger->log<log::Level::Info>("Updated client (", *(_input.Client), ") heartbeat");
        if (hb.TestRequest) {
            if (hb.TestValue.has_value()) {
                if (hb.TestValue.value() != _input.Message[fix::Tag::TestReqId]) {
                    fix::Reject reject;

                    reject.set371_refTagId(_input.Message.at(fix::Tag::MsqSeqNum));
                    reject.set372_refMsgType(_input.Message.at(fix::Tag::MsgType));
                    reject.set373_sessionRejectReason(fix::Reject::ValueOORange);
                    reject.set58_text("Incorrect Test Value");
                    Logger->log<log::Level::Error>("Invalid Test Value: ", hb.TestValue.value(), " - ", _input.Message[fix::Tag::TestReqId], ", for client: (", *(_input.Client), ")");
                    m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject));
                    return false;
                }
                hb.TestRequest = false;
                hb.TestValue = std::nullopt;
            } else {
                Logger->log<log::Level::Fatal>("Missing Test value in memory for client (", *(_input.Client), ")");
                return false;
            }
        }
        hb.Since = _input.ReceiveTime;
        Logger->log<log::Level::Debug>("Heartbeat from (", *(_input.Client), ") moving to TCP output");
        m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(heartbeat));
        return true;
    }

    void HeartBeatHandler::handle(std::stop_token _st)
    {
        while (!_st.stop_requested()) {
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

            ClientStore::Instance().Apply([this, now] (ClientStore::Client _client) {
                if (std::chrono::duration<double>(now - _client->getHeartBeatInfo().Since).count() > PU_HEARTBEAT_TO) {
                    if (!_client->getHeartBeatInfo().TestRequest) {
                        fix::TestRequest test;

                        _client->getHeartBeatInfo().TestValue = std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::floor<std::chrono::seconds>(now));
                        test.set112_testReqID(_client->getHeartBeatInfo().TestValue.value());
                        Logger->log<log::Level::Info>("Sending TestRequest to Client (", *(_client), ") for failed heartbeat");
                        m_tcp_output.append(_client, now, std::move(test));
                    } else {
                        Logger->log<log::Level::Error>("Client (", *(_client), ") failed heartbeat & Test Request");
                        ClientStore::Instance().removeClient(_client);
                    }
                }
            });
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        Logger->log<log::Level::Warning>("Exiting processing thread");
    }
}