#pragma once

#include "Server/ProcessUnit/data/Global.hpp"
#include "Server/ProcessUnit/data/DataRequest.hpp"
#include "Server/OrderBook.hpp"
#include "Server/MDSubscribeRegistery.hpp"

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Shared/ProcessUnit/IProcessUnitStopable.hpp"

namespace pu::market
{
    class DataRequest : public AInputProcess<Context<data::MDSnapshotRequest>>, IProcessUnitStopable
    {
        public:
            DataRequest(MDSubscribeRegistery &_reg, OrderBook &_ob, StringOutputQueue &_tcp_output);
            virtual ~DataRequest() = default;

        protected:
            void setup();

            void onInput(InputType _input) final;

            void onStop();

        private:
            void processSubscribtion(std::stop_token _st);

            MDSubscribeRegistery &m_registery;
            OrderBook &m_ob;

            StringOutputQueue &m_tcp_output;

            std::jthread m_thread;
    };
}