#include <future>

#include "Common/Core/Logger.hpp"
#include "Common/Core/Utils.hpp"
#include "Common/Message/Message.hpp"
#include "Server/Core/Pipeline/Router.hpp"
#include "Server/Core/meta.hpp"

namespace pip
{
    Router::Router(InputRouter &_input, InMarketData &_data, InOutNetwork &_raw)
        : m_input(_input), m_q_data(_data), m_tcp_output(_raw)
    {
    }

    void Router::registerMarket(const std::string &_name, InMarket &_input)
    {
        m_market_input.emplace(_name, _input);
    }

    void Router::runtime(std::stop_token _st)
    {
        std::pair<bool, fix::Reject> reject;
        RouterInput input;

        while (!_st.stop_requested())
        {
            if (!m_input.empty())
            {
                input = m_input.pop_front();
                reject = fix::Header::Verify(input.Message, input.Client->getUserId(), PROVIDER_NAME, input.Client->getSeqNumber());
                if (reject.first) {
                    reject.second.header.set56_TargetCompId(input.Client->getUserId());
                    reject.second.header.set49_SenderCompId(PROVIDER_NAME);
                    Logger::Log("Header verification failed");
                    m_tcp_output.append(std::move(input.Client), std::move(reject.second));
                    continue;
                }
                Logger::Log("Header verification validated");
                Logger::Log("Message from: (", *(input.Client), ") with type: ", input.Message.at(fix::Tag::MsgType));
                switch (input.Message.at("35")[0])
                {
                    case fix::Logon::cMsgType: (void)treatLogon(input);
                        break;
                    case fix::HeartBeat::cMsgType: (void)treatHeartbeat(input);
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
                                case fix::Logout::cMsgType: (void)treatLogout(input);
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

    bool Router::treatLogon(RouterInput &_input)
    {
        Logger::Log("(Logon) Processing message...");
        fix::Logon logon;
        std::pair<bool, fix::Reject> reject = fix::Logon::Verify(_input.Message);

        if (reject.first) {
            Logger::Log("(Logon) Request verification failed");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger::Log("(Logon) Reject moving to TCP output");
            m_tcp_output.append(std::move(_input.Client), std::move(reject.second));
            return false;
        } else if (_input.Client->isLoggedin()) {
            Logger::Log("(Logon) Client (", *(_input.Client), ") already connected");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            reject.second.set58_text("Client already logged in");
            Logger::Log("(Logon) Reject from (", *(_input.Client), ") moving to TCP output");
            m_tcp_output.append(std::move(_input.Client), std::move(reject.second));
            return false;
        }

        _input.Client->login(_input.Message.at(fix::Tag::SenderCompId));
        _input.Client->setSeqNumber(utils::to<size_t>(_input.Message.at(fix::Tag::MsqSeqNum)));

        Logger::Log("(Logon) Client set as logged in as: (", *(_input.Client), ")");
        logon.set98_EncryptMethod("0");
        logon.set108_HeartBtInt(_input.Message.at(fix::Tag::HearBtInt));
        Logger::Log("(Logon) Reply to (", *(_input.Client), ") moving to TCP output");
        m_tcp_output.append(std::move(_input.Client), std::move(logon));
        return true;
    }

    bool Router::treatLogout(RouterInput &_input)
    {
        Logger::Log("(Logout) Processing message...");
        fix::Logout logout;
        std::pair<bool, fix::Reject> reject = fix::Logout::Verify(_input.Message);

        if (reject.first) {
            Logger::Log("(Logout) Request verification failed");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger::Log("(Logout) Reject from (", *(_input.Client), ") moving to TCP output");
            m_tcp_output.append(std::move(_input.Client), std::move(reject.second));
            return false;
        } else if (!_input.Client->isLoggedin()) {
            Logger::Log("(Logout) Client try to logout without begin connected");
            reject.second.set58_text("Client not connected");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger::Log("(Logon) Reject moving to TCP output");
            m_tcp_output.append(std::move(_input.Client), std::move(reject.second));
            return false;
        }

        _input.Client->shouldDisconnect(true);

        Logger::Log("(Logon) Client set as logged out: (", *(_input.Client), ")");
        logout.header.set56_TargetCompId(_input.Client->getUserId());
        Logger::Log("(Logout) Reply to (", *(_input.Client), ") moving to TCP output");
        m_tcp_output.append(std::move(_input.Client), std::move(logout));
        return true;
    }

    bool Router::treatNewOrderSingle(RouterInput &_input)
    {
        Logger::Log("(New Order Single) Processing message...");
        MarketInput data(_input.Client);
        std::pair<bool, fix::Reject> reject = fix::NewOrderSingle::Verify(_input.Message);

        if (reject.first) {
            Logger::Log("(New Order Single) Request verification failed");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger::Log("(Logout) Reject from (", *(_input.Client), ") moving to TCP output");
            m_tcp_output.append(std::move(_input.Client), std::move(reject.second));
            return false;
        }

        data.OrderData.action = OrderBook::Data::Action::Add;
        data.OrderData.type = (_input.Message.at(fix::Tag::Side) == "3") ? OrderType::Bid : OrderType::Ask;
        data.OrderData.price = utils::to<Price>(_input.Message.at(fix::Tag::Price));
        data.OrderData.order.userId = _input.Client->getUserId();
        data.OrderData.order.orderId = _input.Message.at(fix::Tag::ClOrdID);
        data.OrderData.order.quantity = utils::to<Quantity>(_input.Message.at(fix::Tag::OrderQty));

        Logger::Log("(New Order Single) Moving to target OrderBook Container new order: (", data.OrderData.order.orderId,") from: (", data.OrderData.order.userId, ")");
        m_market_input.at(_input.Message.at(fix::Tag::Symbol)).push(std::move(data));
        return true;
    }

    bool Router::treatOrderCancelRequest(RouterInput &_input)
    {
        Logger::Log("(Order Cancel Request) Processing message...");
        MarketInput data(_input.Client);
        std::pair<bool, fix::Reject> reject = fix::OrderCancelRequest::Verify(_input.Message);

        if (reject.first) {
            Logger::Log("(Order Cancel Request) Request verification failed");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger::Log("(Order Cancel Request) Reject from (", *(_input.Client), ") moving to TCP output");
            m_tcp_output.append(std::move(_input.Client), std::move(reject.second));
            return false;
        }

        data.OrderData.action = OrderBook::Data::Action::Cancel;
        data.OrderData.order.orderId = _input.Message.at(fix::Tag::OrigClOrdID);
        data.OrderData.order.userId = _input.Client->getUserId();
        data.OrderData.type = (_input.Message.at(fix::Tag::Side) == "3") ? OrderType::Bid : OrderType::Ask;

        Logger::Log("(New Order Single) Moving to target OrderBook Container cancel order: (", data.OrderData.order.orderId,") from: (", data.OrderData.order.userId, ")");
        m_market_input.at(_input.Message.at(fix::Tag::Symbol)).push(std::move(data));
        return true;
    }

    bool Router::treatOrderCancelReplaceRequest(RouterInput &_input)
    {
        Logger::Log("(Order Cancel Replace) Processing message...");
        MarketInput data(_input.Client);
        std::pair<bool, fix::Reject> reject = fix::OrderCancelReplaceRequest::Verify(_input.Message);

        if (reject.first) {
            Logger::Log("(Order Cancel Replace) Request verification failed");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger::Log("(Order Cancel Replace) Reject from (", *(_input.Client), ") moving to TCP output");
            m_tcp_output.append(std::move(_input.Client), std::move(reject.second));
            return false;
        }

        data.OrderData.action = OrderBook::Data::Action::Modify;
        data.OrderData.order.userId = _input.Client->getUserId();
        data.OrderData.target = _input.Message.at(fix::Tag::OrigClOrdID);
        data.OrderData.order.orderId = _input.Message.at(fix::Tag::ClOrdID);
        data.OrderData.order.quantity = utils::to<Quantity>(_input.Message.at(fix::Tag::OrderQty));
        data.OrderData.price = utils::to<Price>(_input.Message.at(fix::Tag::Price));
        data.OrderData.type = (_input.Message.at(fix::Tag::Side) == "3") ? OrderType::Bid : OrderType::Ask;

        Logger::Log("(New Order Replace) Moving to target OrderBook Container replace order: (", data.OrderData.order.orderId,") from: (", data.OrderData.order.userId, ")");
        m_market_input.at(_input.Message.at(fix::Tag::Symbol)).push(std::move(data));
        return true;
    }

    bool Router::treatUnknown(RouterInput &_input)
    {
        Logger::Log("(Unknown) Processing message...");
        fix::Reject reject;

        Logger::Log("(New Order Single) Rejecting request from client: ", _input.Client->getUserId(), ", with request type: ", _input.Message.at(fix::Tag::MsgType));
        reject.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
        reject.set371_refTagId(fix::Tag::MsgType);
        reject.set373_sessionRejectReason(fix::Reject::NotSupporType);
        reject.set58_text("Unknown message type");
        Logger::Log("(Unknown) Moving Reject Unknown from (", _input.Client->getUserId() ,") to TCP Output");
        m_tcp_output.append(std::move(_input.Client), std::move(reject));
        return true;
    }

    bool Router::treatHeartbeat(RouterInput &_input)
    {
        Logger::Log("(HeartBeat) Processing message...");
        fix::HeartBeat heartbeat;
        std::pair<bool, fix::Reject> reject = fix::HeartBeat::Verify(_input.Message);

        // need to modify user info
        if (reject.first) {
            Logger::Log("(HeartBeat) Request verification failed: ");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger::Log("(HearBeat) Reject from (", *(_input.Client), ") moving to TCP output");
            m_tcp_output.append(std::move(_input.Client), std::move(reject.second));
            return false;
        }
        Logger::Log("(HeartBeat) Validate from client: ", _input.Client->getUserId());
        m_tcp_output.append(std::move(_input.Client), std::move(heartbeat));
        return true;
    }

    bool Router::treatMarketDataRequest(RouterInput &_input)
    {
        MarketDataInput sub;
        std::vector<std::string> types = utils::split<','>(_input.Message.at(fix::Tag::MDEntryType));
        std::vector<std::string> symbols = utils::split<','>(_input.Message.at(fix::Tag::Symbol));

        sub.Client = _input.Client;
        sub.Id = _input.Message.at(fix::Tag::MDReqID);
        sub.SubType = _input.Message.at(fix::Tag::SubscriptionRequestType)[0] - '0';
        sub.Depth = utils::to<size_t>(_input.Message.at(fix::Tag::MarketDepth));
        // if (sub.SubType == 1)
        //     sub.UpdateType = _input.Message.at(fix::Tag::MDUpdateType)[0] - '0';
        for (const auto &_type : types) {
            if (_type == "0")
                sub.Types.push_back(OrderType::Bid);
            else if (_type == "1")
                sub.Types.push_back(OrderType::Ask);
            else
                return false; // build reject
        }
        for (const auto &_sym : symbols)
            sub.Symbols.push_back(std::move(_sym));
        Logger::Log("(MarketDataRequest) Validate from client: ", _input.Client->getUserId());
        m_q_data.push(std::move(sub));
        return true;
    }
}
