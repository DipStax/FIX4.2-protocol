#pragma once

#include "Client/Initiator/Config.hpp"
#include "Client/Initiator/Session.hpp"

#include "Shared/ProcessUnit/AProcessUnitBase.hpp"
#include "Shared/Network/Acceptor.hpp"
#include "Shared/Network/Selector.hpp"
#include "Shared/Thread/Pool.hpp"

#if !defined(TP_BACKHANDLER_NB) || TP_BACKHANDLER_NB < 1
    #define TP_BACKHANDLER_NB 1
#endif

namespace pu
{
    class BackHandler : public AProcessUnitBase
    {
        public:
            BackHandler();
            virtual ~BackHandler() = default;

        protected:
            void runtime(std::stop_token _st);

        private:
            bool process(const std::shared_ptr<net::UnixStream> &_socket);

            net::Acceptor<net::UnixStream> m_acceptor{};
            net::Selector<net::UnixStream> m_selector{};

            ThreadPool<TP_BACKHANDLER_NB> m_tp{};
    };
}