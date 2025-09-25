#include "Server/ProcessUnit/MarketRouter.hpp"

#include "Shared/Message-v2/Parser.hpp"

namespace pu
{
    MarketRouter::MarketRouter(StringOutputQueue &_error)
        : AInputProcess<InputType>("Server/Market-Router"),
        m_error(_error)
    {
    }

    void MarketRouter::registerMarket(const std::string &_symbol, MarketTupleQueue _queue)
    {
        m_markets.emplace(_symbol, _queue);
    }

    void MarketRouter::onInput(InputType _input)
    {
        switch (_input.Header.getPositional<fix42::tag::MsgType>().Value) {
            case fix42::msg::NewOrderSingle::Type:
                processNewOrderSingle(_input);
                break;
            case fix42::msg::OrderCancelRequest::Type:
                processOrderCancelRequest(_input);
                break;
        }
    }

    void MarketRouter::processNewOrderSingle(const InputType &_input)
    {
        xstd::Expected<fix42::msg::NewOrderSingle, fix42::msg::SessionReject> error = fix42::parseMessage<fix42::msg::NewOrderSingle>(_input.Message, _input.Header);

        if (error.has_error()) {
            Logger->log<logger::Level::Info>("Parsing of NewOrderSingle message failed: ", error.error().get<fix42::tag::Text>().Value.value());
            m_error.append(_input.Client, _input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(error.error().to_string()));
            return;
        }
        redirectToMarket(error.value(), _input);
    }

    void MarketRouter::processOrderCancelRequest(const InputType &_input)
    {
        xstd::Expected<fix42::msg::OrderCancelRequest, fix42::msg::SessionReject> error = fix42::parseMessage<fix42::msg::OrderCancelRequest>(_input.Message, _input.Header);

        if (error.has_error()) {
            Logger->log<logger::Level::Info>("Parsing of OrderCancelRequest message failed: ", error.error().get<fix42::tag::Text>().Value.value());
            m_error.append(_input.Client, _input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(error.error().to_string()));
            return;
        }
        redirectToMarket(error.value(), _input);
    }
}