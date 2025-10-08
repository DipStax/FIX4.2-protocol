#include "Server/ProcessUnit/Market/DataRequest.hpp"

namespace pu::market
{
    DataRequest::DataRequest(MDSubscribeRegistery &_reg, OrderBook &_ob, StringOutputQueue &_tcp_output)
        : AInputProcess<InputType>("Server/Market/" + _ob.getSymbol() + "/Data-Request"),
        m_registery(_reg), m_ob(_ob), m_tcp_output(_tcp_output)
    {
    }

    void DataRequest::onInput(InputType _input)
    {
        // todo
    }
}