#include <iomanip>

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
            std::unique_lock lock(m_mutex);

            m_heartbeat.emplace_back(_client, std::chrono::system_clock::now());
        });
        ClientStore::OnRemoveClient([this] (const ClientStore::Client &_client) {
            std::unique_lock lock(m_mutex);

            std::erase_if(m_heartbeat, [_client] (const HeartBeatPair &_pair) {
                return _pair.first == _client;
            });
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
                m_tp.enqueue([this, _input = std::move(m_input.pop_front())] () mutable {
                    process(std::move(_input));
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

    bool HeartBeatHandler::process(InputType &&_input)
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

        {
            std::shared_lock lock(m_mutex);

            // cant be end()
            auto it = std::find_if(m_heartbeat.begin(), m_heartbeat.end(), [_input] (const HeartBeatPair &_pair) {
                return _pair.first == _input.Client;
            });
            Logger->log<log::Level::Info>("Updated client (", *(_input.Client), ") heartbeat");
            it->second = _input.ReceiveTime;
        }

        Logger->log<log::Level::Debug>("Heartbeat from (", *(_input.Client), ") moving to TCP output");
        m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(heartbeat));
        return true;
    }

    void HeartBeatHandler::handle(std::stop_token _st)
    {
        while (!_st.stop_requested()) {
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

            {
                std::shared_lock lock(m_mutex);

                for (const HeartBeatPair &_hb : m_heartbeat) {
                    if (std::chrono::duration<double>(now - _hb.second).count() > PU_HEARTBEAT_TO) {
                        Logger->log<log::Level::Error>("Client (", *(_hb.first), ") failed heartbeat");
                        ClientStore::Instance().removeClient(_hb.first);
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        Logger->log<log::Level::Warning>("Exiting processing thread");
    }
}