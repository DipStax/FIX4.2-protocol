#include <future>

#include "Server/Core/ProcessUnit/data/Market.hpp"
#include "Server/Core/ProcessUnit/Router.hpp"
#include "Server/Core/meta.hpp"

#include "Common/Utils.hpp"
#include "Common/Message/Message.hpp"
#include "Common/Message/Tag.hpp"
#include "Common/Log/Manager.hpp"

namespace pu
{
    Router::Router(InputNetworkOutput &_tcp_output, QueueInputType &_logon, QueueInputType &_logout, QueueInputType &_heartbeat)
        : AInputProcess<InputType>("Server/Master-Router"),
        m_tcp_output(_tcp_output), m_logon_handler(_logon), m_logout_handler(_logout), m_heartbeat_handler(_heartbeat)
    {
    }

    void Router::registerMarket(const std::string &_name, QueueInputType &_input)
    {
        m_market_input.emplace(_name, _input);
    }

    void Router::onInput(InputType _input)
    {
        std::pair<bool, fix::Reject> reject;

        reject = fix::Header::Verify(_input.Message, _input.Client->getUserId(), PROVIDER_NAME, _input.Client->getSeqNumber());

        _input.Client->nextSeqNumber();
        if (reject.first) {
            if (reject.second.contains(fix::Tag::Text))
                Logger->log<logger::Level::Info>("Header verification failed: (", reject.second.get(fix::Tag::RefTagId), ") ", reject.second.get(fix::Tag::Text));
            else
                Logger->log<logger::Level::Warning>("Header verification failed for unknown reason");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
            return;
        }

        Logger->log<logger::Level::Debug>("Header verification validated");
        Logger->log<logger::Level::Info>("Message from: (", *(_input.Client), ") with type: ", _input.Message.at(fix::Tag::MsgType));
        if (_input.Message.at("35")[0] == fix::Logon::cMsgType) {
            m_logon_handler.push(std::move(_input));
        } else if (_input.Client->isLoggedin()) {
            switch (_input.Message.at("35")[0]) {
                case fix::TestRequest::cMsgType:
                case fix::HeartBeat::cMsgType:
                    m_heartbeat_handler.push(std::move(_input));
                    break;
                case fix::Logout::cMsgType: m_logout_handler.push(std::move(_input));
                    break;
                case fix::NewOrderSingle::cMsgType:
                case fix::OrderCancelRequest::cMsgType:
                case fix::OrderCancelReplaceRequest::cMsgType:
                    redirectToMarket(_input);
                    break;
                case fix::MarketDataRequest::cMsgType: // todo
                    break;
                case fix::Reject::cMsgType: treatReject(_input);
                    break;
                default: (void)treatUnknown(_input);
                    break;
            }
        } else {
            (void)treatRequireLogin(_input);
        }
    }

    bool Router::redirectToMarket(const InputType &_input)
    {
        if (!m_market_input.contains(_input.Message.at(fix::Tag::Symbol))) {
            fix::Reject reject{};

            reject.set371_refTagId(fix::Tag::Symbol);
            reject.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            reject.set373_sessionRejectReason(fix::Reject::NotSupporType);
            reject.set58_text("Unknown symbol");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject));
            return false;
        }

        Logger->log<logger::Level::Debug>("(Market Order) Redirecting to correct market router");
        m_market_input.at(_input.Message.at(fix::Tag::Symbol)).push(std::move(_input));
        return true;
    }

    bool Router::treatUnknown(const InputType &_input)
    {
        fix::Reject reject;

        Logger->log<logger::Level::Info>("(Unknow) Rejecting request from client: ", _input.Client->getUserId(), ", with request type: ", _input.Message.at(fix::Tag::MsgType));
        reject.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
        reject.set371_refTagId(fix::Tag::MsgType);
        reject.set373_sessionRejectReason(fix::Reject::NotSupporType);
        reject.set58_text("Unknown message type");
        Logger->log<logger::Level::Debug>("(Unknown) Moving Reject Unknown from (", _input.Client->getUserId() ,") to TCP Output");
        m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject));
        return true;
    }

    bool Router::treatRequireLogin(const InputType &_input)
    {
        Logger->log<logger::Level::Warning>("User not logged in, but try to access login required message type: ", _input.Message.at(fix::Tag::MsgType));
        fix::Reject reject;

        Logger->log<logger::Level::Info>("(New Order Single) Rejecting request from client: ", _input.Client->getUserId(), ", with request type: ", _input.Message.at(fix::Tag::MsgType));
        reject.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
        reject.set371_refTagId(fix::Tag::MsgType);
        reject.set373_sessionRejectReason(fix::Reject::InvalidTag);
        reject.set58_text("Accesing login required features");
        Logger->log<logger::Level::Debug>("(Unknown) Moving Reject Unknown from (", _input.Client->getUserId() ,") to TCP Output");
        m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject));
        return true;
    }

    bool Router::treatReject(const InputType &_input)
    {
        Logger->log<logger::Level::Error>("Rejected message: { refSeqNum: ", _input.Message.at(fix::Tag::RefSeqNum), ", refTagId: ", _input.Message.at(fix::Tag::RefTagId), ", reason: ", _input.Message.at(fix::Tag::SessionRejectReason), " }");
        return true;
    }

    // bool Router::treatMarketDataRequest(InputType &_input)
    // {
        // Context<data::MarketRouterInput> data(_input.Client, _input.ReceiveTime);
        // std::vector<std::string> types = utils::split<','>(_input.Message.at(fix::Tag::MDEntryType));
        // std::vector<std::string> symbols = utils::split<','>(_input.Message.at(fix::Tag::Symbol));
        // sub.type = MarketInput::Type::Refresh;

        // data.Client = _input.Client;
        // data.Id = _input.Message.at(fix::Tag::MDReqID);
        // data.SubType = _input.Message.at(fix::Tag::SubscriptionRequestType)[0] - '0';
        // data.Depth = utils::to<size_t>(_input.Message.at(fix::Tag::MarketDepth));
        // // if (sub.SubType == 1)
        // //     sub.UpdateType = _input.Message.at(fix::Tag::MDUpdateType)[0] - '0';
        // for (const auto &_type : types) {
        //     if (_type == "0")
        //         data.Types.push_back(OrderType::Bid);
        //     else if (_type == "1")
        //         data.Types.push_back(OrderType::Ask);
        //     else
        //         return false; // build reject
        // }
        // for (const auto &_sym : symbols)
        //     m_market_input.at(_sym).append(std::move(sub)); // reject if not found
        // Logger->log<logger::Level::Info>("(MarketDataRequest) Validate from client: ", _input.Client->getUserId());
        // return true;
    // }
}
