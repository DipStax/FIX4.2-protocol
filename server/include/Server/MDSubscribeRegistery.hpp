#pragma once

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Server/InternalClient.hpp"

#include "Shared/Core/Core.hpp"

class MDSubscribeRegistery
{
    public:
        struct Registery
        {
            uint32_t depth;
            fix42::MarketDataEntryType entry;

            bool operator==(const Registery &_reg) const noexcept;
        };

        struct RegisteryHash
        {
            size_t operator()(const Registery &_reg) const noexcept;
        };

        MDSubscribeRegistery() = default;
        ~MDSubscribeRegistery() = default;

        [[nodiscard]] static bool has(const std::string &_id);


        void subscribe(const std::shared_ptr<InternalClient> &_client, const std::string &_id, uint32_t _depth, fix42::MarketDataEntryType _type);
        bool unsubscribe(const std::string &_id, uint32_t _depth, fix42::MarketDataEntryType _type);
        // unsubscribeAll(const std::shared_ptr<InternalClient> &_client)

    private:
        struct Subscription
        {
            std::string id;
            std::shared_ptr<InternalClient> client;
        };

        struct SubscriptionVector
        {
            std::shared_mutex mutex;
            std::vector<Subscription> subs;
        };

        inline static std::mutex m_id_mutex{};
        inline static std::unordered_set<std::string> m_used_id{};

        std::mutex m_reg_mutex;
        std::unordered_map<Registery, SubscriptionVector, RegisteryHash> m_registery;
};