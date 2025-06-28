#include <future>

#include "Server/Core/ProcessUnit/Network/UDPOutput.hpp"

#include "Common/Log/Manager.hpp"

namespace pu
{
    UdpOutputNetwork::UdpOutputNetwork(uint32_t _port)
        : AProcessUnit<InputType>("Server/NET/UDP-Output")
    {
        m_socket.connect("localhost", _port);
        if (!m_socket.setBroadcast(true))
            Logger->log<logger::Level::Error>("Failed to setup broadcast, crashing after first action");
    }

    UdpOutputNetwork::QueueInputType &UdpOutputNetwork::getInput()
    {
        return m_input;
    }

    void UdpOutputNetwork::runtime(std::stop_token _st)
    {
        Logger->log<logger::Level::Info>("Launching process unit runtime");

        while (!_st.stop_requested()) {
            auto now = std::chrono::steady_clock::now();

            for (size_t it = 0; it < UDP_MAX_MSG && !m_input.empty(); it++) {
                // Logger->log<logger::Level::Info>("New notification to be broadcast: ", m_input.front());
                m_message.emplace_back(now, m_input.pop_front());
            }
            if (m_message.size())
                Logger->log<logger::Level::Debug>("Broadcasting message number: ", m_message.size());
            for (const auto &[_, _val] : m_message) {
                (void)m_socket.send(reinterpret_cast<const std::byte *>(&_val), sizeof(data::UDPPackage));
            }
            clean();
            std::this_thread::sleep_for(std::chrono::seconds(UDP_TICK));
        }
        Logger->log<logger::Level::Info>("Exiting process unit runtime");
    }

    void UdpOutputNetwork::clean()
    {
        constexpr const auto clean_after =  std::chrono::seconds(UDP_CLEAR);
        auto now = std::chrono::steady_clock::now();
        size_t cleaned = 0;

        for (auto it = m_message.begin(); it != m_message.end();) {
            if (std::chrono::duration_cast<std::chrono::seconds>(now - it->first).count() >= 2) {
                // Logger->log<logger::Level::Info>("Cleaning message:", it->second);
                it = m_message.erase(it);
                cleaned++;
            } else {
                it++;
            }
        }
        if (cleaned)
            Logger->log<logger::Level::Info>("Cleaned ", cleaned, " messages from the UDP queue, new size: ", m_message.size());
    }
}