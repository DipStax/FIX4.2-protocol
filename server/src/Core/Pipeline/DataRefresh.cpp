#include "Server/Core/Pipeline/DataRefresh.hpp"

namespace pip
{
    DataRefresh::DataRefresh(OrderBook &_ob, InOutNetwork &_output)
        : m_ob(_ob), m_tcp_output(_output)
    {
    }

    DataRefresh::QueueInputType &DataRefresh::getInput()
    {
        return m_input;
    }

    void DataRefresh::runtime(std::stop_token _st)
    {
        Logger::SetThreadName(THIS_THREAD_ID, "DataRefresh");
        Context<MarketInput> input;

        while (!_st.stop_requested()) {
            if (!m_input.empty()) {
                input = m_input.pop_front();
                process(input);
            }
        }
    }

    void DataRefresh::process(Context<MarketInput> &_input)
    {
        MarketRefreshInputData &data = std::get<MarketRefreshInputData>(_input.Data);

        if (data.SubType == 0) {
            Logger::Log("[DataRefresh] SubType is refresh");
            OrderBook::Subscription sub;
            fix::MarketDataSnapshotFullRefresh message;

            message.set262_mDReqID(data.Id);
            Logger::Log("[DataRefresh] Checking for symbol: ", m_ob.getSymbol());
            for (OrderType _type : data.Types) {
                Logger::Log("[DataRefresh] Checking symbol: ", m_ob.getSymbol(), ", for type: ", (int)_type);
                sub.depth = data.Depth;
                sub.type = _type;
                message += m_ob.refresh(sub);
            }
            m_ob.cache_flush();
            message.set55_symbol(m_ob.getSymbol());
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(message));
            Logger::Log("[DataRefresh] Refresh done");
        } else if (data.SubType == 1) {
            Logger::Log("[DataRefresh] SubType is subscribe");
            OrderBook::Subscription sub;

            sub.depth = data.Depth;
            sub.Id = data.Id;
            std::vector<OrderBook::Subscription> &subs = _input.Client->subscribe(m_ob.getSymbol());
            fix::MarketDataSnapshotFullRefresh message;

            for (OrderType _type : data.Types) {
                Logger::Log("[DataRefresh] (Subscribe) to: ", m_ob.getSymbol(), " with type: ", _type);
                sub.type = _type;
                subs.push_back(sub);
            }
            message.set55_symbol(m_ob.getSymbol());
            message.set262_mDReqID(data.Id);
            message += m_ob.refresh(sub);
            // std::cout << "subcribtion size after: " << data.Client.subscribe(_sym).size() << std::endl;
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(message));
        } else {
            Logger::Log("[DataRefresh] SubType is unsubscribe");
            std::vector<OrderBook::Subscription> &subs = _input.Client->subscribe(m_ob.getSymbol());

            for (OrderType _type : data.Types)
                for (std::vector<OrderBook::Subscription>::iterator it = subs.begin(); it != subs.end();)
                    if (data.Id == it->Id && _type == it->type)
                        subs.erase(it);
            // what to send???
        }
    }
}