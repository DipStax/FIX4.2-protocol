#include "Server/Core/Pipeline/Notification.hpp"
#include "Common/Message/MarketDataIncrementalRefresh.hpp"
#include "Common/Message/MarketDataSnapshotFullRefresh.hpp"

namespace pip
{
    Notification::Notification(const std::string &_name, OrderBook &_ob, std::vector<ClientSocket> &_clients)
        : m_name(_name), m_ob(_ob), m_clients(_clients)
    {
    }

    Notification::~Notification()
    {
        (void)stop();
    }

    bool Notification::start()
    {
        if (!m_running)
            tstart(this);
        Logger::Log("[Notification] Running: ", m_running);
        return m_running;
    }

    void Notification::loop()
    {
        Logger::SetThreadName(THIS_THREAD_ID, "Notification Generator - " + m_name);
        auto update = std::chrono::system_clock::now();

        while (m_running) {
            auto now = std::chrono::system_clock::now();
            auto update_diff = std::chrono::duration_cast<std::chrono::seconds>(now - update);

            if (update_diff.count() >= NOTIF_UPDATE_TO) {
                Logger::Log("[Refresh] incremenetal - start");
                for (auto &_client : m_clients) {
                    const ClientSocket::Subs &subs = _client.subscribe(m_name);
                    fix::MarketDataIncrementalRefresh notif;
                    Logger::Log("[Refresh] Incremental For client: ", _client.User, ", size of the query: ", subs.size());

                    if (!subs.empty()) {
                        for (const auto &_sub : subs)
                            notif += m_ob.update(_sub);
                        notif.header.set34_msgSeqNum(std::to_string((_client.SeqNumber)++));
                        notif.header.set49_SenderCompId(PROVIDER_NAME);
                        notif.header.set56_TargetCompId(_client.User);
                        Logger::Log("[Refresh] Subscribtion of user: ", _client.User, ", generated");
                        if (_client.getSocket()->is_open()) {
                            Logger::Log("[Refresh] Subscribtion notification for: ", _client.User, ", send");
                            _client.getSocket()->send(notif.to_string());
                        }
                    }
                }
                m_ob.cache_flush();
                Logger::Log("[Refresh] Incremenetal - done");
                update = now;
            }
        }
    }
}