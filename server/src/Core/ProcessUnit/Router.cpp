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
        : m_tcp_output(_tcp_output), m_logon_handler(_logon), m_logout_handler(_logout), m_heartbeat_handler(_heartbeat), Logger(logger::Manager::newLogger("Router-master"))
    {
    }

    void Router::registerMarket(const std::string &_name, QueueInputType &_input)
    {
        m_market_input.emplace(_name, _input);
    }

    Router::QueueInputType &Router::getInput()
    {
        return m_input;
    }

    void Router::runtime(std::stop_token _st)
    {
        std::pair<bool, fix::Reject> reject;
        InputType input;

        while (!_st.stop_requested()) {
            while (!m_input.empty()) {
                input = m_input.pop_front();
                reject = fix::Header::Verify(input.Message, input.Client->getUserId(), PROVIDER_NAME, input.Client->getSeqNumber());
                if (reject.first) {
                    if (reject.second.contains(fix::Tag::Text))
                        Logger->log<logger::Level::Info>("Header verification failed: (", reject.second.get(fix::Tag::RefTagId), ") ", reject.second.get(fix::Tag::Text));
                    else
                        Logger->log<logger::Level::Warning>("Header verification failed for unknown reason");
                    m_tcp_output.append(input.Client, input.ReceiveTime, std::move(reject.second));
                    continue;
                }
                input.Client->nextSeqNumber();

                Logger->log<logger::Level::Debug>("Header verification validated");
                Logger->log<logger::Level::Info>("Message from: (", *(input.Client), ") with type: ", input.Message.at(fix::Tag::MsgType));
                if (input.Message.at("35")[0] == fix::Logon::cMsgType) {
                    m_logon_handler.push(std::move(input));
                } else if (input.Client->isLoggedin()) {
                    switch (input.Message.at("35")[0]) {
                        case fix::TestRequest::cMsgType:
                        case fix::HeartBeat::cMsgType:
                            m_heartbeat_handler.push(std::move(input));
                            break;
                        case fix::Logout::cMsgType: m_logout_handler.push(std::move(input));
                            break;
                        case fix::NewOrderSingle::cMsgType:
                        case fix::OrderCancelRequest::cMsgType:
                        case fix::OrderCancelReplaceRequest::cMsgType:
                            redirectToMarket(input);
                            break;
                        case fix::MarketDataRequest::cMsgType: // todo
                            break;
                        case fix::Reject::cMsgType: treatReject(input);
                            break;
                        default: (void)treatUnknown(input);
                            break;
                    }
                } else {
                    (void)treatRequireLogin(input);
                }
            }
        }
    }

    void Router::redirectToMarket(InputType &_input)
    {
        Logger->log<logger::Level::Debug>("(Market Order) Redirecting to correct market router");
        m_market_input.at(_input.Message[fix::Tag::Symbol]).push(std::move(_input));
    }

    bool Router::treatUnknown(InputType &_input)
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

    bool Router::treatRequireLogin(InputType &_input)
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

    bool Router::treatReject(InputType &_input)
    {
        Logger->log<logger::Level::Error>("Rejected message: { refSeqNum: ", _input.Message.at(fix::Tag::RefSeqNum), ", refTagId: ", _input.Message.at(fix::Tag::RefTagId), ", reason: ", _input.Message.at(fix::Tag::SessionRejectReason), " }");
        return true;
    }

    bool Router::treatMarketDataRequest(InputType &_input)
    {
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
    }
}
