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
    /// @brief Process Unit handling network connection and message header processing
    class TCPInputNetwork : public AProcessUnitBase
    {
        public:
            /// @brief Construct the default implementation of the network input unit
            /// @param _output Standard message output
            /// @param _error Error messsage output
            /// @param _port Port on wich the server will listen
            TCPInputNetwork(UnparsedMessageQueue &_output, StringOutputQueue &_error, uint32_t _port);
            virtual ~TCPInputNetwork() = default;

        protected:
            /// @brief Lookup for incoming connection and message and process them
            /// @param _st stop token
            void runtime(std::stop_token _st) final;

        private:
            /// @brief Process the incoming event of a specific client
            /// @param _client Client with a pending network event
            /// @return `true` if the client should be disconnected and remove from the selector, otherwise `false`
            bool process(const ClientStore::Client &_client);

            /// @brief Parse and build the header. Send it to the appropriet output:
            ///
            ///     - `m_output` if the build and parse was succesfull
            ///
            ///     - `m_error` if the build failed, the message reject is directly build and send though this canal
            /// @param _client Client sending the data to be process
            /// @param _data Data receive by the client
            void buildHeader(const ClientStore::Client &_client, const std::string &_data);
            /// @brief Verify that all the required tag are present in the header
            /// @param _header Header to verify
            /// @param _tagset set of the presently parsed tag
            /// @return If a tag is missing build a `fix::RejectError` to then build a reject message, otherwise a `std::nullopt` is return
            [[nodiscard]] std::optional<fix::RejectError> verifyHeader(const fix42::Header &_header, const std::unordered_set<uint16_t> &_tagset);
            /// @brief Build a FIX 4.2 Session Reject message from internal `RejectError` structure
            /// @param _error Base error structure
            /// @param _tagset set of the presently parsed tag
            /// @param _header Header from which the error come from
            /// @param _client Client sending the message
            /// @return The builded reject message
            [[nodiscard]] fix42::msg::SessionReject BuildRejectFromError(const fix::RejectError &_error, const std::unordered_set<uint16_t> &_tagset, const fix42::Header &_header, const ClientStore::Client &_client);
            /// @brief Verify recursivly that all tag provided as template parameter are in the set repersenting the present tag
            /// @tparam Tag Current tag to verify
            /// @tparam ...RemainTags Remaining Tag to verify
            /// @param _tagset Set of tag parsed
            /// @return If a tag is missing build a `fix::RejectError` to then build a reject message, otherwise a `std::nullopt` is return
            template<fix::TagName Tag, fix::TagName ...RemainTags>
            [[nodiscard]] static inline std::optional<fix::RejectError> HeaderVerifyPresence(const std::unordered_set<uint16_t> &_tagset);

            /// @brief Sum all the character in _data (+ 1 for '\0') to create the check sum
            /// @param _data String to count on
            /// @return The accumulation of all the character in _data
            static uint32_t CountCheckSum(const std::string &_data);
            /// @brief Verify the calculated check sum correspond to the value of the last tag, if succed the last tag is removed
            /// @param _checksum Calculated check sum for parsing
            /// @param _map Map of the non header field
            /// @return If the check sum is inccorect or doesn't exist build a `fix::RejectError` to then build a reject message, otherwise a `std::nullopt` is return
            std::optional<fix::RejectError> verifyCheckSum(uint32_t _checksum, fix::MapMessage &_map) const;

            UnparsedMessageQueue &m_output;             ///< Standard output for incoming message
            StringOutputQueue &m_error;                 ///< Error output to TCP/IP network output

            net::Acceptor<net::INetTcp> m_acceptor{};   ///< TCP/IP acceptor
            net::Selector<net::INetTcp> m_selector{};   ///< TCP/IP selector
    };
}

#include "Server/ProcessUnit/Network/TCPInput.inl"