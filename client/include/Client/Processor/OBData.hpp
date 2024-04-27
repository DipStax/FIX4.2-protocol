#pragma once

#include "Client/Processor/Common/DataRequest.hpp"
#include "Common/Message/MarketDataRequest.hpp"

namespace proc
{
    class OBData : public com::DataRequest<IEntry>
    {
        public:
            OBData() = default;
            ~OBData() = default;

            [[nodiscard]] virtual bool handle(const Entry &_entry, const Context &_ctx) const override final;

            [[nodiscard]] virtual std::optional<fix::Message> process(const Entry &_entry, Context &_ctx) override final;

        protected:
            [[nodiscard]] std::optional<fix::MarketDataRequest> buildRequest(const Entry &_entry);
            [[nodiscard]] std::optional<fix::Message> buildForUnsub(const Entry &_entry);

            [[nodiscard]] bool applyModification(char _param, const std::string &_value, fix::MarketDataRequest &_refresh);

            [[nodiscard]] bool setSymbol(const std::string &_symbol, fix::MarketDataRequest &_request);
            [[nodiscard]] bool setDepth(const std::string &_depth, fix::MarketDataRequest &_request);
            [[nodiscard]] bool setSide(const std::string &_side, fix::MarketDataRequest &_request);
            [[nodiscard]] bool setReqId(const std::string &_reqid, fix::MarketDataRequest &_request);
    };
}