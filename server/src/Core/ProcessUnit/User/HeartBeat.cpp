#include "Server/Core/ProcessUnit/User/HeartBeat.hpp"

#include "Common/Message/HeartBeat.hpp"
#include "Common/Message/Tag.hpp"

namespace pu::user
{
    HeartBeatHandler::HeartBeatHandler(InOutNetwork &_tcp_output)
        : m_tcp_output(_tcp_output)
    {
    }

    HeartBeatHandler::QueueInputType &HeartBeatHandler::getInput()
    {
        return m_input;
    }

    void HeartBeatHandler::runtime(std::stop_token _st)
    {
        while (!_st.stop_requested()) {
            while (!m_input.empty()) {
                m_tp.enqueue([this, _input = std::move(m_input.pop_front())] () mutable {
                    Logger::Log("(HeartBeat) Processing message...");
                    fix::HeartBeat heartbeat;
                    std::pair<bool, fix::Reject> reject = fix::HeartBeat::Verify(_input.Message);

                    // need to modify user info
                    if (reject.first) {
                        Logger::Log("(HeartBeat) Request verification failed: ");
                        reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
                        Logger::Log("(HearBeat) Reject from (", *(_input.Client), ") moving to TCP output");
                        m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
                        return false;
                    }
                    Logger::Log("(HeartBeat) Validate from client: ", _input.Client->getUserId());
                    m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(heartbeat));
                    return true;
                });
            }
        }
    }
}