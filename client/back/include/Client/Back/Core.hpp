
// #include "Client/Back/ProcessUnit/Market/Execution.hpp"
#include "Client/Back/ProcessUnit/Network/TcpInput.hpp"
#include "Client/Back/ProcessUnit/Network/TcpOutput.hpp"
#include "Client/Back/ProcessUnit/HeaderValidation.hpp"
// #include "Client/Back/ProcessUnit/User/HeartBeat.hpp"
// #include "Client/Back/ProcessUnit/User/Auth.hpp"
// #include "Client/Back/ProcessUnit/FixBuilder.hpp"
#include "Client/Back/ProcessUnit/Router.hpp"

#include "Shared/ProcessUnit/ProcessUnit.hpp"
#include "Shared/Log/ILogger.hpp"
#include "Shared/Network/Socket.hpp"

class Core
{
    public:
        Core();
        ~Core();

        bool start();
        void stop();
    private:
        bool m_running = false;

        UnparsedMessageQueue m_tmp{};

        std::shared_ptr<net::INetTcp> m_server;

        ProcessUnit<pu::TcpOutputNetwork> m_tcp_output;

        // ProcessUnit<pu::FixBuilder> m_builder;

        // ProcessUnit<pu::HeartBeatHandler> m_heartbeat;
        // ProcessUnit<pu::AuthHandler> m_auth;
        // ProcessUnit<pu::Execution> m_execution;

        ProcessUnit<pu::Router> m_router;
        ProcessUnit<pu::HeaderValidation> m_header_validation;

        ProcessUnit<pu::TcpInputNetwork> m_tcp_input;


        std::unique_ptr<logger::ILogger> Logger = nullptr;
};