#pragma once

#include "Server/Core/Pipeline/Naming.hpp"
#include "Server/Core/OrderBook.hpp"
#include "Server/Core/Pipeline/IProcessUnit.hpp"

#ifndef NOTIF_REFRESH_TO
    #define NOTIF_REFRESH_TO 5000
#endif

#ifndef NOTIF_UPDATE_TO
    #define NOTIF_UPDATE_TO 5000
#endif

namespace pip
{
    class Notification : public IProcessUnitBase
    {
        public:
            Notification(const std::string &_name, OrderBook &_ob, InOutNetwork &_tcp);
            virtual ~Notification() = default;

        protected:
            void runtime(std::stop_token _st);

        private:
            const std::string m_name;

            InOutNetwork &m_tcp_output;
            OrderBook &m_ob;
    };
}