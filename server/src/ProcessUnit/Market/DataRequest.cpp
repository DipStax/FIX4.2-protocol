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
        fix42::list::MDEntriesSnapshot::DataTuple mdentry{};
        fix42::msg::MarketDataSnapshotFullRefresh mdsnapshot{};
        std::vector<std::pair<Price, Quantity>> snapshot = m_ob.getSnapshot(_input.depth, fix42::Side::Sell);

        for (const std::pair<Price, Quantity> &_pair : snapshot) {
            fix::get<fix42::tag::MDEntryType>(mdentry).Value = fix42::MarketDataEntryType::Offer;
            fix::get<fix42::tag::MDEntryPx>(mdentry).Value = _pair.first;
            fix::get<fix42::tag::MDEntrySize>(mdentry).Value = _pair.second;
            mdsnapshot.getList<fix42::tag::NoMDEntries>().add(mdentry);
        }
        snapshot = m_ob.getSnapshot(_input.depth, fix42::Side::Buy);
        for (const std::pair<Price, Quantity> &_pair : snapshot) {
            fix::get<fix42::tag::MDEntryType>(mdentry).Value = fix42::MarketDataEntryType::Bid;
            fix::get<fix42::tag::MDEntryPx>(mdentry).Value = _pair.first;
            fix::get<fix42::tag::MDEntrySize>(mdentry).Value = _pair.second;
            mdsnapshot.getList<fix42::tag::NoMDEntries>().add(mdentry);
        }
        mdsnapshot.get<fix42::tag::MDReqID>().Value = _input.id;
        mdsnapshot.get<fix42::tag::Symbol>().Value = m_ob.getSymbol();
        m_tcp_output.append(_input.Client, _input.ReceiveTime, fix42::msg::MarketDataSnapshotFullRefresh::Type, std::move(mdsnapshot.to_string()));
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