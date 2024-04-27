#include "Client/Processor/Common/DataRequest.hpp"
#include "Common/Core/Logger.hpp"
#include "Common/Core/Utils.hpp"
#include "Common/Message/MarketDataSnapshotFullRefresh.hpp"
#include "Common/Message/MarketDataRequest.hpp"
#include "Common/Message/MarketDataIncrementalRefresh.hpp"

namespace proc::com
{
    template<class ...Ts>
    requires IsBuildFIX<Ts...>
    std::optional<fix::Message> DataRequest<Ts...>::build(char _tag, const Context &_ctx) const
    {
        std::ignore = _ctx;

        if (_tag == fix::MarketDataSnapshotFullRefresh::cMsgType)
            return buildFullRefresh();
        else if (_tag == fix::MarketDataIncrementalRefresh::cMsgType)
            return buildIncrRefresh();
        return {};
    }

    template<class ...Ts>
    requires IsBuildFIX<Ts...>
    fix::Message DataRequest<Ts...>::buildFullRefresh() const
    {
        fix::MarketDataRequest request;
        const std::vector<std::string> symbols{ MARKET_NAME };
        std::string lsymbols;

        request.set146_noRelatedSym(std::to_string(symbols.size()));
        for (const auto &_sym : symbols)
            lsymbols += _sym + ",";
        if (!symbols.empty())
            lsymbols.erase(lsymbols.end());
        request.set55_symbol(lsymbols);
        request.set262_mDReqID(utils::id());
        request.set263_subscriptionRequestType("0");
        request.set264_marketDepth("0");
        request.set267_noMDEntryTypes("2");
        request.set269_mDEntryType("0,1");
        Logger::Log("[TCPInput] Sending a data full refresh request");
        return request;
    }

    template<class ...Ts>
    requires IsBuildFIX<Ts...>
    fix::Message DataRequest<Ts...>::buildIncrRefresh() const
    {
        fix::MarketDataRequest request;
        const std::vector<std::string> symbols{ MARKET_NAME };
        std::string lsymbols;

        request.set146_noRelatedSym(std::to_string(symbols.size()));
        for (const auto &_sym : symbols)
            lsymbols += _sym + ",";
        if (!symbols.empty())
            lsymbols.erase(lsymbols.end());
        request.set55_symbol(lsymbols);
        request.set262_mDReqID(utils::id());
        request.set263_subscriptionRequestType("1");
        request.set264_marketDepth("0");
        request.set267_noMDEntryTypes("2");
        request.set269_mDEntryType("0,1");
        Logger::Log("[TCPInput] Sending a data incremental refresh request");
        return request;
    }
}