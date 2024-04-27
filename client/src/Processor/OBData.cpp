#include <cstring>

#include "Client/Processor/OBData.hpp"
#include "Common/Core/Utils.hpp"

namespace proc
{
    bool OBData::handle(const Entry &_entry, const Context &_ctx) const
    {
        if (_entry.args.size() == 0 || (
            std::strcmp(_entry.args.at(0), "fresh") != 0 &&
            std::strcmp(_entry.args.at(0), "sub") != 0 &&
            std::strcmp(_entry.args.at(0), "unsub") != 0))
            return false;
        if (!_ctx.Loggin) {
            Logger::Log("You need to be connected to execute this command");
            return false;
        }
        return true;
    }

    std::optional<fix::Message> OBData::process(const Entry &_entry, Context &_ctx)
    {
        std::ignore = _ctx;
        std::optional<fix::MarketDataRequest> refresh{};

        if (std::strcmp(_entry.args.at(0), "fresh") == 0) {
            refresh = buildRequest(_entry);
            if (refresh.has_value())
                refresh.value().set263_subscriptionRequestType("0");
        } else if (std::strcmp(_entry.args.at(0), "sub") == 0) {
            refresh = buildRequest(_entry);
            if (refresh.has_value())
                refresh.value().set263_subscriptionRequestType("1");
        } else if (std::strcmp(_entry.args.at(0), "unsub") == 0) {
            return buildForUnsub(_entry);
        }
        return refresh;
    }

    std::optional<fix::MarketDataRequest> OBData::buildRequest(const Entry &_entry)
    {
        fix::MarketDataRequest refresh;
        int param = 0;

        while (param != -1) {
            param = getopt(_entry.args.size(), const_cast<char * const *>(_entry.args.data()), "S:s:d:i:");
            switch (param) {
                case ':':
                case '?':
                    return {};
            };
            if (param != -1 && !applyModification(param, optarg, refresh)) {
                Logger::Log("failed to set param: ", param);
                return {};
            }
        };
        Logger::Log("[OBData] Request send succefully");
        return refresh;
    }

    std::optional<fix::Message> OBData::buildForUnsub(const Entry &_words)
    {
        // todo
        return {};
    }

    bool OBData::applyModification(char _param, const std::string &_value, fix::MarketDataRequest &_refresh)
    {
        Logger::Log("[OBData] Apply the param: ", _param);
        switch (_param) {
            case 'S': return setSymbol(_value, _refresh);
            case 's': return setSide(_value, _refresh);
            case 'd': return setDepth(_value, _refresh);
            case 'i': return setReqId(_value, _refresh);
        };
        return false;
    }

    bool OBData::setSymbol(const std::string &_symbol, fix::MarketDataRequest &_request)
    {
        Logger::Log("[OBData] Setting symbol 1");
        const std::vector<std::string> sym{ MARKET_NAME };
        Logger::Log("[OBData] Setting symbol");
        _request.set146_noRelatedSym("1");
        _request.set55_symbol(_symbol);
        Logger::Log("[OBData] Setting symbol to: ", _symbol);
        return std::find_if(sym.begin(), sym.end(), [_symbol] (const std::string &_sym) {
                return _symbol == _sym;
            }) != sym.end();
    }

    bool OBData::setDepth(const std::string &_depth, fix::MarketDataRequest &_request)
    {
        if (_depth == "full") {
            Logger::Log("[OBData] Setting depth to: 0");
            _request.set264_marketDepth("0");
        } else if (_depth == "top") {
            Logger::Log("[OBData] Setting depth to: 1");
            _request.set264_marketDepth("1");
        } else if (utils::is_numeric(_depth)) {
            Logger::Log("[OBData] Setting depth to: ", _depth);
            _request.set264_marketDepth(_depth);
        } else {
            Logger::Log("[OBData] Setting depth failed");
            return false;
        }
        return true;
    }

    bool OBData::setSide(const std::string &_side, fix::MarketDataRequest &_request)
    {
        if (_side == "both") {
            _request.set267_noMDEntryTypes("2");
            _request.set269_mDEntryType("0,1");
            Logger::Log("[OBData] Setting NoMDEntryType to: 2");
            Logger::Log("[OBData] Setting MDEntryType to: 0,1");
        } else {
            _request.set267_noMDEntryTypes("1");
            Logger::Log("[OBData] Setting NoMDEntryType to: 2");
            if (_side == "ask") {
                Logger::Log("[OBData] Setting MDEntryType to: 0");
                _request.set269_mDEntryType("0");
            } else if (_side == "bid") {
                Logger::Log("[OBData] Setting MDEntryType to: 1");
                _request.set269_mDEntryType("1");
            } else {
                Logger::Log("[OBData] Setting MDEntryType failed");
                return false;
            }
        }
        return true;
    }

    bool OBData::setReqId(const std::string &_reqid, fix::MarketDataRequest &_request)
    {
        _request.set262_mDReqID(_reqid);
        Logger::Log("[OBData] Setting MDReqID: ", _reqid);
        return true;
    }

}
