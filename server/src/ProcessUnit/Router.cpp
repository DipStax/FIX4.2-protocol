#include "Server/ProcessUnit/Router.hpp"
#include "Server/meta.hpp"
#include "Server/Config.hpp"

#include "Shared/Utils/Utils.hpp"
#include "Shared/Log/Manager.hpp"

namespace pu
{
    Router::Router(UnparsedMessageQueue &_logon, UnparsedMessageQueue &_logout, UnparsedMessageQueue &_heartbeat, UnparsedMessageQueue &_market_router, UnparsedMessageQueue &_md_router, StringOutputQueue &_error)
        : AInputProcess<InputType>("Server/Master-Router"),
        m_logon_handler(_logon),
        m_logout_handler(_logout),
        m_heartbeat_handler(_heartbeat),
        m_market_router(_market_router),
        m_marketdata_router(_md_router),
        m_error(_error)
    {
    }

    // void Router::registerMarket(const std::string &_name, QueueInputType &_input)
    // {
    //     m_market_input.emplace(_name, _input);
    // }

    void Router::onInput(InputType _input)
    {
        if (_input.Header.getPositional<fix42::tag::MsgType>().Value == fix42::msg::Logon::Type) {
            Logger->log<logger::Level::Info>("Moving message to Logon process unit");
            m_logon_handler.push(std::move(_input));
        } else if (_input.Client->isLoggedin()) {
            switch (_input.Header.getPositional<fix42::tag::MsgType>().Value) {
                case fix42::msg::TestRequest::Type:
                case fix42::msg::HeartBeat::Type:
                    m_heartbeat_handler.push(std::move(_input));
                    break;
                case fix42::msg::Logout::Type:
                    m_logout_handler.push(std::move(_input));
                    break;
                case fix42::msg::NewOrderSingle::Type:
                case fix42::msg::OrderCancelRequest::Type:
                // case fix::OrderCancelReplaceRequest::cMsgType:
                    m_market_router.push(std::move(_input));
                    break;
                case fix42::msg::MarketDataRequest::Type:
                    m_marketdata_router.push(std::move(_input));
                    break;
                case fix42::msg::SessionReject::Type:
                    treatReject(_input);
                    break;
                default:
                    treatUnknown(_input);
                    break;
            }
        } else {
            treatRequireLogin(_input);
        }
    }

    // bool Router::redirectToMarket(const InputType &_input)
    // {
    //     if (!m_market_input.contains(_input.Message.at(fix::Tag::Symbol))) {
    //         fix::Reject reject{};

    //         reject.set371_refTagId(fix::Tag::Symbol);
    //         reject.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
    //         reject.set373_sessionRejectReason(fix::Reject::NotSupporType);
    //         reject.set58_text("Unknown symbol");
    //         m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject));
    //         return false;
    //     }

    //     Logger->log<logger::Level::Debug>("(Market Order) Redirecting to correct market router");
    //     m_market_input.at(_input.Message.at(fix::Tag::Symbol)).push(std::move(_input));
    //     return true;
    // }

    void Router::treatUnknown(const InputType &_input)
    {
        fix42::msg::SessionReject reject;

        reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
        reject.get<fix42::tag::RefTagId>().Value = fix42::tag::MsgType;
        reject.get<fix42::tag::SessionRejectReason>().Value = fix42::RejectReasonSession::ValueOutOfRange;
        reject.get<fix42::tag::Text>().Value = "Unknown message type";
        Logger->log<logger::Level::Info>("Rejecting request from client: ", _input.Client->getUserId(), ", reason: ", reject.get<fix42::tag::Text>().Value.value());
        m_error.append(_input.Client, _input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(reject.to_string()));
    }

    void Router::treatRequireLogin(const InputType &_input)
    {
        fix42::msg::SessionReject reject;

        reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
        reject.get<fix42::tag::RefTagId>().Value = fix42::tag::MsgType;
        reject.get<fix42::tag::SessionRejectReason>().Value = fix42::RejectReasonSession::ValueOutOfRange;
        reject.get<fix42::tag::Text>().Value = "Accesing login required features will not logged in";
        Logger->log<logger::Level::Warning>("User (", _input.Client->getUserId(), ") not logged in, but try to access login required message type: ", _input.Header.get<fix42::tag::MsgSeqNum>().Value);
        m_error.append(_input.Client, _input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(reject.to_string()));
    }

    void Router::treatReject(const InputType &_input)
    {
        Logger->log<logger::Level::Error>("Received a reject message from client: ", _input.Client->getUserId(), ", reject info: "); // todo log
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
