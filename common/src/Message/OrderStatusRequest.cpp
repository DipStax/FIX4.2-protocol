#include "Common/Message/OrderStatusRequest.hpp"

namespace fix
{
    OrderStatusRequest::OrderStatusRequest()
    {
        header.set35_MsgType(MsgType);
    }

    // void OrderStatusRequest::set37_OrderID(const std::string &_val)
    // {
    //     m_params.append({ "37", _val });
    // }

    void OrderStatusRequest::set11_ClOrdID(const std::string &_val)
    {
        m_params.append({ "11", _val });
    }

    // void OrderStatusRequest::set41_OrigClOrdID(const std::string &_val)
    // {
    //     m_params.append({ "41", _val });
    // }

    void OrderStatusRequest::set55_Symbol(const std::string &_val)
    {
        m_params.append({ "55", _val });
    }

    void OrderStatusRequest::set54_Side(const std::string &_val)
    {
        m_params.append({ "54", _val });
    }
}