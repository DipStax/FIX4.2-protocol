#pragma once

#include <memory>

#include "Client/Back/ProcessUnit/data/Global.hpp"

#include "Shared/ProcessUnit/AProcessUnitBase.hpp"
#include "Shared/Log/ILogger.hpp"
#include "Shared/Network/Selector.hpp"
#include "Shared/Network/Socket.hpp"

namespace pu
{
    class TcpInputNetwork : public AProcessUnitBase
    {
        public:
            using Socket = std::shared_ptr<net::INetTcp>;

            TcpInputNetwork(Socket _server, UnparsedMessageQueue &_output, StringOutputQueue &_error);
            virtual ~TcpInputNetwork() = default;

        protected:
            void runtime(std::stop_token _st)  final;

        private:
            void buildHeader(const std::string &_data);
            std::optional<fix::RejectError> verifyHeader(const fix42::Header &_header, const std::unordered_set<uint16_t> &_tagset);
            fix42::msg::SessionReject BuildRejectFromError(const fix::RejectError &_error, const std::unordered_set<uint16_t> &_tagset, const fix42::Header &_header);
            template<fix::TagName Tag, fix::TagName ...RemainTags>
            [[nodiscard]] static inline std::optional<fix::RejectError> HeaderVerifyPresence(const std::unordered_set<uint16_t> &_tagset);

            uint32_t CountCheckSum(const std::string &_data);
            std::optional<fix::RejectError> verifyCheckSum(uint32_t _checksum, fix::MapMessage &_map) const;

            Socket m_server;

            UnparsedMessageQueue &m_output;
            StringOutputQueue &m_error;

            net::Selector<net::INetTcp> m_selector;
    };
}

#include "Client/Back/ProcessUnit/Network/TcpInput.inl"