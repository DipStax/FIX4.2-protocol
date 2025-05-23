#pragma once

#include "Server/Core/ProcessUnit/interface/IProcessUnitBase.hpp"
#include "Server/Core/ProcessUnit/data/Global.hpp"

#ifndef NOTIF_UPDATE_TO
    #define NOTIF_UPDATE_TO 5000
#endif

namespace pu::market
{
    class Notification : public IProcessUnitBase
    {
        public:
            Notification(OrderBook &_ob, InputNetworkOutput &_tcp);
            virtual ~Notification() = default;

        protected:
            std::string getThreadName() const;

            void runtime(std::stop_token _st);

        private:
            const std::string m_name;

            InputNetworkOutput &m_tcp_output;
            OrderBook &m_ob;
    };
}