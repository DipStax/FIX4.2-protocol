#include "Server/MDSubscribeRegistery.hpp"

bool MDSubscribeRegistery::Registery::operator==(const Registery &_reg) const noexcept
{
    return depth == _reg.depth && entry == _reg.entry;
}

size_t MDSubscribeRegistery::RegisteryHash::operator()(const Registery &_reg) const noexcept
{
    size_t h1 = std::hash<uint32_t>{}(_reg.depth);
    size_t h2 = std::hash<int>{}(static_cast<int>(_reg.entry));

    return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
}

bool MDSubscribeRegistery::has(const std::string &_id)
{
    std::lock_guard<std::mutex> lock(m_id_mutex);

    return m_used_id.contains(_id);
}

void MDSubscribeRegistery::subscribe(const std::shared_ptr<InternalClient> &_client, const std::string &_id, uint32_t _depth, fix42::MarketDataEntryType _type)
{
    {
        Registery reg{
            .depth = _depth,
            .entry = _type
        };
        std::lock_guard<std::mutex> lock_reg(m_reg_mutex);

        m_registery.try_emplace(reg);

        std::unique_lock lock(m_registery.at(reg).mutex);

        m_registery.at(reg).subs.push_back(Subscription{_id, _client});
    }
    std::unique_lock lock(m_id_mutex);

    m_used_id.emplace(_id);
}

bool MDSubscribeRegistery::unsubscribe(const std::string &_id, uint32_t _depth, fix42::MarketDataEntryType _type)
{
    Registery reg{
        .depth = _depth,
        .entry = _type
    };
    std::lock_guard<std::mutex> lock_reg(m_reg_mutex);

    if (m_registery.contains(reg)) {
        SubscriptionVector &subs = m_registery.at(reg);
        std::unique_lock lock(subs.mutex);

        std::erase_if(subs.subs, [_id] (const Subscription &_sub) {
            return _sub.id == _id;
        });
        return true;
    }
    return false;
}