#include "Server/ProcessUnit/MarketRouter.hpp"

namespace pu
{
    template<class T>
    void MarketRouter::redirectToMarket(const T &_msg, const InputType &_input)
    {
        const std::string &symbol = _msg.template get<fix42::tag::Symbol>().Value;

        if (m_markets.contains(symbol)) {
            std::get<MessageQueue<T> &>(m_markets.at(symbol)).append(_input.Client, _input.ReceiveTime, std::move(_input.Header), std::move(_msg));
        } else {
            fix42::msg::BusinessReject reject{};

            reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
            reject.get<fix42::tag::RefMsgType>().Value = _input.Header.getPositional<fix42::tag::MsgType>().Value;
            reject.get<fix42::tag::BusinessRejectReason>().Value = fix42::RejectReasonBusiness::UnknownSecurity;
            reject.get<fix42::tag::Text>().Value = "Unknown symbol";
            m_error.append(_input.Client, _input.ReceiveTime, fix42::msg::BusinessReject::Type, std::move(reject.to_string()));
        }
    }
}