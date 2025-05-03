#include <future>

#include "Server/Core/ProcessUnit/UDPOutNetwork.hpp"

#include "Common/Core/Logger.hpp"

namespace pu
{
    UDPOutNetwork::UDPOutNetwork(uint32_t _port)
    {
        if (!m_socket.broadcastOn(_port))
            Logger::Log("[UDPOutNetwork] Failed to setup broadcast, crashing after first action");
    }

    UDPOutNetwork::QueueInputType &UDPOutNetwork::getInput()
    {
        return m_input;
    }

    std::string UDPOutNetwork::getThreadName() const
    {
        return "UDP Broadcast";
    }

    void UDPOutNetwork::runtime(std::stop_token _st)
    {
        while (!_st.stop_requested()) {
            auto now = std::chrono::steady_clock::now();

            for (size_t it = 0; it < UDP_MAX_MSG && !m_input.empty(); it++) {
                Logger::Log("[UDPOutNetwork] New notification to be broadcast: ", m_input.front());
                m_message.emplace_back(now, m_input.pop_front());
            }
            if (m_message.size())
                Logger::Log("[UDPOutNetwork] Broadcasting message number: ", m_message.size());
            for (const auto &[_, _val] : m_message) {
                (void)m_socket.broadcast(reinterpret_cast<const uint8_t*>(&_val), sizeof(data::UDPPackage));
            }
            clean();
            sleep(UDP_TICK);
        }
    }

    void UDPOutNetwork::clean()
    {
        constexpr const auto clean_after =  std::chrono::seconds(UDP_CLEAR);
        auto now = std::chrono::steady_clock::now();
        size_t cleaned = 0;

        for (auto it = m_message.begin(); it != m_message.end();) {
            if (std::chrono::duration_cast<std::chrono::seconds>(now - it->first).count() >= 2) {
                Logger::Log("[UDPOutNetwork] Cleaning message:", it->second);
                it = m_message.erase(it);
                cleaned++;
            } else {
                it++;
            }
        }
        if (cleaned)
            Logger::Log("[UDPOutNetwork] Cleaned ", cleaned, " messages from the UDP queue, new size: ", m_message.size());
    }
}