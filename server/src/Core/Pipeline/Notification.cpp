#include "Server/Core/Pipeline/Notification.hpp"
#include "Common/Message/MarketDataIncrementalRefresh.hpp"
#include "Common/Message/MarketDataSnapshotFullRefresh.hpp"

namespace pip
{
    Notification::Notification(const std::string &_name, OrderBook &_ob, InOutNetwork &_tcp)
        : m_name(_name), m_tcp_output(_tcp), m_ob(_ob)
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
                    const InternalClient::Subs &subs = _client->subscribe(m_name);
                    fix::MarketDataIncrementalRefresh notif;

                    Logger::Log("[Refresh] Incremental For client: ", _client->User, ", size of the query: ", subs.size());
                    if (!subs.empty()) {
                        for (const auto &_sub : subs)
                            notif += m_ob.update(_sub);
                        notif.header.set34_msgSeqNum(std::to_string((_client->SeqNumber)++));
                        notif.header.set49_SenderCompId(PROVIDER_NAME);
                        notif.header.set56_TargetCompId(_client->User);
                        m_tcp_output.append(_client, notif);
                    }
                });
                m_ob.cache_flush();
                Logger::Log("[Refresh] Incremenetal - done");
                update = now;
            }
        }
    }
}