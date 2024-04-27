#pragma once

#include "Server/Core/Pipeline/Core.hpp"
#include "Server/Core/Pipeline/Naming.hpp"
#include "Server/Core/OrderBook.hpp"

#ifndef NOTIF_REFRESH_TO
    #define NOTIF_REFRESH_TO 5000
#endif

#ifndef NOTIF_UPDATE_TO
    #define NOTIF_UPDATE_TO 5000
#endif

namespace pip
{
    class Notification : public Pipeline<Notification>
    {
        public:
            Notification(const std::string &_name, OrderBook &_ob, std::vector<ClientSocket> &_clients);
            ~Notification();

            /// @brief Run the pipeline
            /// @return Return true if the pipeline as correctly started else false.
            [[nodiscard]] bool start();

            /// @brief Core function of the pipeline determining it's behavior
            void loop();

        private:
            const std::string m_name;

            OrderBook &m_ob;

            std::vector<ClientSocket> &m_clients;
    };
}