#include "Server/ProcessUnit/MarketDataRouter.hpp"

#include "Shared/Message-v2/Parser.hpp"

namespace pu
{
    MarketDataRouter::MarketDataRouter(StringOutputQueue &_tcp_output)
        : AInputProcess<InputType>("Server/MarketData-Router"),
        m_tcp_output(_tcp_output)
    {
    }

    void MarketDataRouter::registerMarket(const std::string &_symbol, MarketDataTupleQueue &_queue)
    {
        m_market.emplace(_symbol, _queue);
    }

    void MarketDataRouter::onInput(InputType _input)
    {
        xstd::Expected<fix42::msg::MarketDataRequest, fix42::msg::SessionReject> error = fix42::parseMessage<fix42::msg::MarketDataRequest>(_input.Message, _input.Header);

        if (error.has_error()) {
            Logger->log<logger::Level::Info>("Parsing of MarketDataRequest message failed: ", error.error().get<fix42::tag::Text>().Value.value());
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(error.error().to_string()));
            return;
        }

        switch (error.value().get<fix42::tag::SubscriptionRequestType>().Value) {
            case fix42::SubscirptionType::Snapshot:
                handleSnapshot(_input, error.value());
                break;
            case fix42::SubscirptionType::Subscirbe:
                handleSubscribe(_input, error.value());
                break;
            case fix42::SubscirptionType::Unsubscribe:
                break;
        }
    }

    void MarketDataRouter::handleSnapshot(const InputType &_input, const fix42::msg::MarketDataRequest &_mdreq)
    {
        data::MDSnapshotRequest snapshot{_mdreq.get<fix42::tag::MarketDepth>().Value, _mdreq.get<fix42::tag::MDReqID>().Value};

        for (const auto &_entry : _mdreq.getList<fix42::tag::NoMDEntryTypes>())
            snapshot.entries.push_back(fix::get<fix42::tag::MDEntryType>(_entry).Value);

        for (const auto &_symbol : _mdreq.getList<fix42::tag::NoRelatedSym>()) {
            ts::Queue<Context<data::MDSnapshotRequest>> &market = std::get<ts::Queue<Context<data::MDSnapshotRequest>> &>(m_market.at(fix::get<fix42::tag::Symbol>(_symbol).Value));

            market.append(_input.Client, _input.ReceiveTime, snapshot);
        }
    }

    void MarketDataRouter::handleSubscribe(const InputType &_input, const fix42::msg::MarketDataRequest &_mdreq)
    {
        if (!_mdreq.get<fix42::tag::MDUpdateType>().Value.has_value()) {
            fix42::msg::BusinessReject reject{};

            reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
            reject.get<fix42::tag::RefMsgType>().Value = _input.Header.getPositional<fix42::tag::MsgType>().Value;
            reject.get<fix42::tag::BusinessRejectRefId>().Value = _mdreq.get<fix42::tag::MDReqID>().Value;
            reject.get<fix42::tag::BusinessRejectReason>().Value = fix42::RejectReasonBusiness::CondReqFieldMissing;
            reject.get<fix42::tag::Text>().Value = "Expected a update type for subscribtion";
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::BusinessReject::Type, std::move(reject.to_string()));
        } else if (MDSubscribeRegistery::has(_mdreq.get<fix42::tag::MDReqID>().Value)) {
            fix42::msg::BusinessReject reject{};

            reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
            reject.get<fix42::tag::RefMsgType>().Value = _input.Header.getPositional<fix42::tag::MsgType>().Value;
            reject.get<fix42::tag::BusinessRejectRefId>().Value = _mdreq.get<fix42::tag::MDReqID>().Value;
            reject.get<fix42::tag::BusinessRejectReason>().Value = fix42::RejectReasonBusiness::Other;
            reject.get<fix42::tag::Text>().Value = "Market Data Request Id already used";
            m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::BusinessReject::Type, std::move(reject.to_string()));
        }

        handleSnapshot(_input, _mdreq);

        if (_mdreq.get<fix42::tag::MDUpdateType>().Value.value() == fix42::MarketDataUpdateType::IncrementalRefresh) {
            for (const auto &_symbol : _mdreq.getList<fix42::tag::NoRelatedSym>()) {
                MDSubscribeRegistery &registery = std::get<MDSubscribeRegistery &>(m_market.at(fix::get<fix42::tag::Symbol>(_symbol).Value));

                for (const auto &_entry : _mdreq.getList<fix42::tag::NoMDEntryTypes>())
                    registery.subscribe(_input.Client, _mdreq.get<fix42::tag::MDReqID>().Value, _mdreq.get<fix42::tag::MarketDepth>().Value, fix::get<fix42::tag::MDEntryType>(_entry).Value );
            }
        }
    }
}