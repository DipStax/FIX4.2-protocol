#pragma once

#include <optional>
#include <unordered_set>

#include "Server/ProcessUnit/data/Global.hpp"
#include "Server/ClientStore.hpp"
#include "Server/meta.hpp"

#include "FIX-Message/RejectError.hpp"
#include "Shared/Message-v2/Message.hpp"
#include "Shared/ProcessUnit/AProcessUnitBase.hpp"
#include "Shared/Network/Selector.hpp"

namespace pu
{
    class TCPInputNetwork : public AProcessUnitBase
    {
        public:
            TCPInputNetwork(InputRouter &_output, InputNetworkOutput &_error, uint32_t _port);
            virtual ~TCPInputNetwork() = default;

        protected:
            void runtime(std::stop_token _st) final;

        private:
            bool process(const ClientStore::Client &_client);

            void buildHeader(const ClientStore::Client &_client, const std::string &_data);
            [[nodiscard]] std::optional<fix::RejectError> verifyHeader(const fix42::Header &_header, const std::unordered_set<uint16_t> &_tagset);

            [[nodiscard]] fix42::msg::SessionReject BuildRejectFromError(const fix::RejectError &_error, const std::unordered_set<uint16_t> &_tagset, const fix42::Header &_header, const ClientStore::Client &_client);
            template<fix::TagName Tag, fix::TagName ...RemainTags>
            [[nodiscard]] static inline std::optional<fix::RejectError> HeaderVerifyPresence(const std::unordered_set<uint16_t> &_tagset);

            InputRouter &m_output;
            InputNetworkOutput &m_error;

            net::Acceptor<net::INetTcp> m_acceptor{};
            net::Selector<net::INetTcp> m_selector{};
    };
}

#include "Server/ProcessUnit/Network/TCPInput.inl"