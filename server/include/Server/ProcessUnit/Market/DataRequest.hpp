#pragma once

#include "Server/ProcessUnit/data/Global.hpp"
#include "Server/ProcessUnit/data/DataRequest.hpp"
#include "Server/OrderBook.hpp"
#include "Server/MDSubscribeRegistery.hpp"

#include "Shared/ProcessUnit/AInputProcess.hpp"

namespace pu::market
{
    class DataRequest : public AInputProcess<Context<data::MDSnapshotRequest>>
    {
        public:
            DataRequest(MDSubscribeRegistery &_reg, OrderBook &_ob, StringOutputQueue &_tcp_output);
            virtual ~DataRequest() = default;

        protected:
            void onInput(InputType _input) final;

        private:
            MDSubscribeRegistery &m_registery;
            OrderBook &m_ob;

            StringOutputQueue &m_tcp_output;
    };
}