#include <future>

#include "Server/Core/ProcessUnit/Router.hpp"
#include "Server/Core/meta.hpp"

#include "Common/Core/Logger.hpp"
#include "Common/Core/Utils.hpp"
#include "Common/Message/Message.hpp"

namespace pu
{
    Router::Router(InOutNetwork &_tcp_output, QueueInputType &_logon, QueueInputType &_logout)
        : m_tcp_output(_tcp_output), m_logon_handler(_logon), m_logout_handler(_logout)
    {
    }

    void Router::registerMarket(const std::string &_name, InMarket &_input)
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
        Context<RouterInput> input;

        while (!_st.stop_requested())
        {
            while (!m_input.empty())
            {
                input = m_input.pop_front();
                reject = fix::Header::Verify(input.Message, input.Client->getUserId(), PROVIDER_NAME, input.Client->getSeqNumber());
                if (reject.first) {
                    Logger::Log("Header verification failed");
                    m_tcp_output.append(input.Client, input.ReceiveTime, std::move(reject.second));
                    continue;
                }
                Logger::Log("Header verification validated");
                Logger::Log("Message from: (", *(input.Client), ") with type: ", input.Message.at(fix::Tag::MsgType));
                switch (input.Message.at("35")[0])
                {
                    case fix::Logon::cMsgType: m_logon_handler.push(std::move(input));
                        break;
                    case fix::HeartBeat::cMsgType: (void)treatHeartbeat(input);
                        break;
                    case fix::Logout::cMsgType: m_logout_handler.push(std::move(input));
                        break;
                    default:
                        if (input.Client->isLoggedin()) {
                            switch (input.Message.at("35")[0])
                            {
                                case fix::NewOrderSingle::cMsgType: (void)treatNewOrderSingle(input);
                                    break;
                                case fix::OrderCancelRequest::cMsgType: (void)treatOrderCancelRequest(input);
                                    break;
                                case fix::OrderCancelReplaceRequest::cMsgType: (void)treatOrderCancelReplaceRequest(input);
                                    break;
                                case fix::MarketDataRequest::cMsgType: (void) treatMarketDataRequest(input);
                                    break;
                                default: (void)treatUnknown(input);
                                    break;
                            }
                        } else {
                            Logger::Log("User logged in, but try to access login required message type: ", input.Message.at(fix::Tag::MsgType));
                            (void)treatUnknown(input);
                        }
                }
            }
        }
    }

    bool Router::treatNewOrderSingle(Context<RouterInput> &_input)
    {
        Logger::Log("(New Order Single) Processing message...");
        Context<MarketInput> nos(_input.Client, _input.ReceiveTime);
        std::pair<bool, fix::Reject> reject = fix::NewOrderSingle::Verify(_input.Message);
        OrderBook::Data &data = std::get<OrderBook::Data>(nos.Data);

        if (reject.first) {
            Logger::Log("(New Order Single) Request verification failed");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger::Log("(Logout) Reject from (", *(_input.Client), ") moving to TCP output");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
            return false;
        }

        data.action = OrderBook::Data::Action::Add;
        data.type = (_input.Message.at(fix::Tag::Side) == "3") ? OrderType::Bid : OrderType::Ask;
        data.price = utils::to<Price>(_input.Message.at(fix::Tag::Price));
        data.order.userId = _input.Client->getUserId();
        data.order.orderId = _input.Message.at(fix::Tag::ClOrdID);
        data.order.quantity = utils::to<Quantity>(_input.Message.at(fix::Tag::OrderQty));

        Logger::Log("(New Order Single) Moving to target OrderBook Container new order: (", data.order.orderId,") from: (", data.order.userId, ")");
        m_market_input.at(_input.Message.at(fix::Tag::Symbol)).push(std::move(nos));
        return true;
    }

    bool Router::treatOrderCancelRequest(Context<RouterInput> &_input)
    {
        Logger::Log("(Order Cancel Request) Processing message...");
        Context<MarketInput> ocr(_input.Client, _input.ReceiveTime);
        std::pair<bool, fix::Reject> reject = fix::OrderCancelRequest::Verify(_input.Message);
        OrderBook::Data &data = std::get<OrderBook::Data>(ocr.Data);

        if (reject.first) {
            Logger::Log("(Order Cancel Request) Request verification failed");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger::Log("(Order Cancel Request) Reject from (", *(_input.Client), ") moving to TCP output");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
            return false;
        }

        data.action = OrderBook::Data::Action::Cancel;
        data.order.orderId = _input.Message.at(fix::Tag::OrigClOrdID);
        data.order.userId = _input.Client->getUserId();
        data.type = (_input.Message.at(fix::Tag::Side) == "3") ? OrderType::Bid : OrderType::Ask;

        Logger::Log("(New Order Single) Moving to target OrderBook Container cancel order: (", data.order.orderId,") from: (", data.order.userId, ")");
        m_market_input.at(_input.Message.at(fix::Tag::Symbol)).push(std::move(ocr));
        return true;
    }

    bool Router::treatOrderCancelReplaceRequest(Context<RouterInput> &_input)
    {
        Logger::Log("(Order Cancel Replace) Processing message...");
        Context<MarketInput> ocrr(_input.Client, _input.ReceiveTime);
        std::pair<bool, fix::Reject> reject = fix::OrderCancelReplaceRequest::Verify(_input.Message);
        OrderBook::Data &data = std::get<OrderBook::Data>(ocrr.Data);

        if (reject.first) {
            Logger::Log("(Order Cancel Replace) Request verification failed");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger::Log("(Order Cancel Replace) Reject from (", *(_input.Client), ") moving to TCP output");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
            return false;
        }

        data.action = OrderBook::Data::Action::Modify;
        data.order.userId = _input.Client->getUserId();
        data.target = _input.Message.at(fix::Tag::OrigClOrdID);
        data.order.orderId = _input.Message.at(fix::Tag::ClOrdID);
        data.order.quantity = utils::to<Quantity>(_input.Message.at(fix::Tag::OrderQty));
        data.price = utils::to<Price>(_input.Message.at(fix::Tag::Price));
        data.type = (_input.Message.at(fix::Tag::Side) == "3") ? OrderType::Bid : OrderType::Ask;

        Logger::Log("(New Order Replace) Moving to target OrderBook Container replace order: (", data.order.orderId,") from: (", data.order.userId, ")");
        m_market_input.at(_input.Message.at(fix::Tag::Symbol)).push(std::move(ocrr));
        return true;
    }

    bool Router::treatUnknown(Context<RouterInput> &_input)
    {
        Logger::Log("(Unknown) Processing message...");
        fix::Reject reject;

        Logger::Log("(New Order Single) Rejecting request from client: ", _input.Client->getUserId(), ", with request type: ", _input.Message.at(fix::Tag::MsgType));
        reject.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
        reject.set371_refTagId(fix::Tag::MsgType);
        reject.set373_sessionRejectReason(fix::Reject::NotSupporType);
        reject.set58_text("Unknown message type");
        Logger::Log("(Unknown) Moving Reject Unknown from (", _input.Client->getUserId() ,") to TCP Output");
        m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject));
        return true;
    }

    bool Router::treatHeartbeat(Context<RouterInput> &_input)
    {
        Logger::Log("(HeartBeat) Processing message...");
        fix::HeartBeat heartbeat;
        std::pair<bool, fix::Reject> reject = fix::HeartBeat::Verify(_input.Message);

        // need to modify user info
        if (reject.first) {
            Logger::Log("(HeartBeat) Request verification failed: ");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger::Log("(HearBeat) Reject from (", *(_input.Client), ") moving to TCP output");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
            return false;
        }
        Logger::Log("(HeartBeat) Validate from client: ", _input.Client->getUserId());
        m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(heartbeat));
        return true;
    }

    bool Router::treatMarketDataRequest(Context<RouterInput> &_input)
    {
        Context<MarketInput> sub(_input.Client, _input.ReceiveTime);
        std::vector<std::string> types = utils::split<','>(_input.Message.at(fix::Tag::MDEntryType));
        std::vector<std::string> symbols = utils::split<','>(_input.Message.at(fix::Tag::Symbol));
        sub.type = MarketInput::Type::Refresh;
        MarketRefreshInputData &data = std::get<MarketRefreshInputData>(sub.Data);

        sub.Client = _input.Client;
        data.Id = _input.Message.at(fix::Tag::MDReqID);
        data.SubType = _input.Message.at(fix::Tag::SubscriptionRequestType)[0] - '0';
        data.Depth = utils::to<size_t>(_input.Message.at(fix::Tag::MarketDepth));
        // if (sub.SubType == 1)
        //     sub.UpdateType = _input.Message.at(fix::Tag::MDUpdateType)[0] - '0';
        for (const auto &_type : types) {
            if (_type == "0")
                data.Types.push_back(OrderType::Bid);
            else if (_type == "1")
                data.Types.push_back(OrderType::Ask);
            else
                return false; // build reject
        }
        for (const auto &_sym : symbols)
            m_market_input.at(_sym).append(std::move(sub)); // reject if not found
        Logger::Log("(MarketDataRequest) Validate from client: ", _input.Client->getUserId());
        return true;
    }
}
