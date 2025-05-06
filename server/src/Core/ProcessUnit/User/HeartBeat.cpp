#include <iomanip>

#include "Server/Core/ProcessUnit/User/HeartBeat.hpp"

#include "Common/Message/HeartBeat.hpp"
#include "Common/Message/Tag.hpp"

namespace pu::user
{
    HeartBeatHandler::HeartBeatHandler(InOutNetwork &_tcp_output)
        : m_tcp_output(_tcp_output)
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

    std::string HeartBeatHandler::getThreadName() const
    {
        return "HeartBeat Handler";
    }

    void HeartBeatHandler::runtime(std::stop_token _st)
    {
        while (!_st.stop_requested()) {
            while (!m_input.empty()) {
                m_tp.enqueue([this, _input = std::move(m_input.pop_front())] () mutable {
                    process(std::move(_input));
                });
            }
        }
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
        Logger::Log("Processing message...");
        fix::HeartBeat heartbeat;
        std::pair<bool, fix::Reject> reject = fix::HeartBeat::Verify(_input.Message);

        if (reject.first) {
            Logger::Log("Request verification failed: ");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger::Log("Reject from (", *(_input.Client), ") moving to TCP output");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
            return false;
        }

        {
            std::shared_lock lock(m_mutex);

            // cant be end()
            auto it = std::find_if(m_heartbeat.begin(), m_heartbeat.end(), [_input] (const HeartBeatPair &_pair) {
                return _pair.first == _input.Client;
            });
            Logger::Log("Updated client (", *(_input.Client), ") heartbeat");
            it->second = _input.ReceiveTime;
        }

        m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(heartbeat));
        return true;
    }

    void HeartBeatHandler::handle(std::stop_token _st)
    {
        Logger::SetThreadName(THIS_THREAD_ID, "HeartBeat Suppressor");

        while (!_st.stop_requested()) {
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

            {
                std::shared_lock lock(m_mutex);

                for (const HeartBeatPair &_hb : m_heartbeat) {
                    if (std::chrono::duration<double>(now - _hb.second).count() > PU_HEARTBEAT_TO) {
                        Logger::Log("Client (", *(_hb.first), ") failed heartbeat");
                        ClientStore::Instance().removeClient(_hb.first);
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        Logger::Log("Exiting thread");
    }
}