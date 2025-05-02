#include "Server/Core/Pipeline/Notification.hpp"
#include "Common/Message/MarketDataIncrementalRefresh.hpp"
#include "Common/Message/MarketDataSnapshotFullRefresh.hpp"

namespace pip
{
    Notification::Notification(OrderBook &_ob, InOutNetwork &_tcp)
        : m_tcp_output(_tcp), m_ob(_ob)
    {
    }

    void Notification::runtime(std::stop_token _st)
    {
        auto update = std::chrono::system_clock::now();

        while (!_st.stop_requested()) {
            auto now = std::chrono::system_clock::now();
            auto update_diff = std::chrono::duration_cast<std::chrono::seconds>(now - update);

            if (update_diff.count() >= NOTIF_UPDATE_TO) {
                Logger::Log("[Refresh] incremenetal - start");
                ClientStore::Instance().Apply([*this] (ClientStore::Client _client) {
                    Logger::Log("[Refresh] Looking for subscribtion of ", m_ob.getSymbol(), " for user: ", _client == nullptr);
                    if (_client->isSubscribeTo(m_ob.getSymbol())) {
                        const InternalClient::Subs &subs = _client->subscribe(m_ob.getSymbol());
                        fix::MarketDataIncrementalRefresh notif;

                        Logger::Log("[Refresh] Incremental For client: ", _client->getUserId(), ", size of the query: ", subs.size());
                        if (!subs.empty()) {
                            for (const auto &_sub : subs)
                                notif += m_ob.update(_sub);
                            m_tcp_output.append(_client, std::chrono::system_clock::now(), std::move(notif));
                        }
                    }
                });
                m_ob.cache_flush();
                Logger::Log("[Refresh] Incremenetal - done");
                update = now;
            }
        }
    }
}