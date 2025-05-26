#include "Server/Core/ProcessUnit/Market/Notification.hpp"

#include "Common/Message/MarketDataIncrementalRefresh.hpp"
#include "Common/Message/MarketDataSnapshotFullRefresh.hpp"
#include "Common/Log/Manager.hpp"

namespace pu::market
{
    Notification::Notification(OrderBook &_ob, InputNetworkOutput &_tcp)
        : m_tcp_output(_tcp), m_ob(_ob), Logger(log::Manager::newLogger("Market/" + m_ob.getSymbol() + "/Notification"))
    {
    }

    void Notification::runtime(std::stop_token _st)
    {
        auto update = std::chrono::system_clock::now();

        while (!_st.stop_requested()) {
            auto now = std::chrono::system_clock::now();
            auto update_diff = std::chrono::duration_cast<std::chrono::seconds>(now - update);

            if (update_diff.count() >= NOTIF_UPDATE_TO) {
                Logger->log<log::Level::Info>("[Refresh] incremenetal - start");
                ClientStore::Instance().Apply([this] (ClientStore::Client _client) {
                    if (_client->isSubscribeTo(m_ob.getSymbol())) {
                        const InternalClient::Subs &subs = _client->subscribe(m_ob.getSymbol());
                        fix::MarketDataIncrementalRefresh notif;

                        Logger->log<log::Level::Debug>("[Refresh] Incremental For client: ", _client->getUserId(), ", size of the query: ", subs.size());
                        if (!subs.empty()) {
                            for (const auto &_sub : subs)
                                notif += m_ob.update(_sub);
                            m_tcp_output.append(_client, std::chrono::system_clock::now(), std::move(notif));
                        }
                    }
                });
                m_ob.cache_flush();
                Logger->log<log::Level::Info>("[Refresh] Incremenetal - done");
                update = now;
            }
        }
    }
}