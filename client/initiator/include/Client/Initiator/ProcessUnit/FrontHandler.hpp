#pragma once

#include "Client/Initiator/Config.hpp"

#include "Shared/ProcessUnit/AProcessUnitBase.hpp"
#include "Shared/Network/Acceptor.hpp"
#include "Shared/Network/Selector.hpp"
#include "Shared/Thread/Pool.hpp"

#if !defined(TP_FRONTHANDLER_NB) || TP_FRONTHANDLER_NB < 1
    #define TP_FRONTHANDLER_NB 1
#endif

namespace pu
{
    class FrontHandler : public AProcessUnitBase
    {
        public:
            FrontHandler();
            virtual ~FrontHandler();

        protected:
            void runtime(std::stop_token _st);

        private:
            net::Acceptor<net::INetTcp> m_acceptor{};
            net::Selector<net::INetTcp> m_selector{};

            ThreadPool<TP_FRONTHANDLER_NB> m_tp;
    };
}