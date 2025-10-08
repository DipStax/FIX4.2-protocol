#include "Server/ProcessUnit/Market/DataRequest.hpp"

namespace pu::market
{
    DataRequest::DataRequest(MDSubscribeRegistery &_reg, OrderBook &_ob, StringOutputQueue &_tcp_output)
        : AInputProcess<InputType>("Server/Market/" + _ob.getSymbol() + "/Data-Request"),
        m_registery(_reg), m_ob(_ob), m_tcp_output(_tcp_output)
    {
    }

    void DataRequest::setup()
    {
        m_thread = std::jthread(&DataRequest::processSubscribtion, this);
    }

    void DataRequest::onInput(InputType _input)
    {
    }

    void DataRequest::onStop()
    {
        if (m_thread.joinable()) {
            Logger->log<logger::Level::Info>("Requesting stop on the worker thread");
            m_thread.request_stop();
            m_thread.join();
            Logger->log<logger::Level::Debug>("Worker thread joined");
        }
    }

    void DataRequest::processSubscribtion(std::stop_token _st)
    {
        while (!_st.stop_requested()) {
            
        }
    }
}