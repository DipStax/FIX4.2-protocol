#pragma once

#include "Server/ProcessUnit/User/Logon.hpp"
#include "Server/ProcessUnit/User/Logout.hpp"
#include "Server/ProcessUnit/User/HeartBeat.hpp"

#include "Server/ProcessUnit/Network/TCPInput.hpp"
#include "Server/ProcessUnit/Network/TCPOutput.hpp"
#include "Server/ProcessUnit/Network/UDPOutput.hpp"

#include "Server/ProcessUnit/MarketContainer.hpp"

#include "Server/ProcessUnit/Router.hpp"

#include "Shared/ProcessUnit/ProcessUnit.hpp"

#include "Shared/Log/Manager.hpp"

/**
 * @class Core
 * @brief Main server orchestrator for the FIX 4.2 protocol implementation
 * 
 * The Core class serves as the central coordinator for all server components.
 * It manages the lifecycle of various ProcessUnits that handle different aspects
 * of the FIX protocol implementation including network I/O, user sessions,
 * message routing, and market operations.
 * 
 * Architecture:
 * - Uses ProcessUnit pattern for modular component management
 * - Implements a pipeline architecture for message flow
 * - Coordinates multiple threaded components for concurrent processing
 */
class Core
{
    public:
        /**
         * @brief Constructs the Core server with specified network ports
         * @param _tcp_port Port number for TCP connections (FIX protocol communication)
         * @param _udp_port Port number for UDP connections (market data broadcast)
         */
        Core(uint32_t _tcp_port, uint32_t _udp_port);
        
        /**
         * @brief Destructor - ensures clean shutdown of all components
         */
        ~Core();

        /**
         * @brief Starts the server and all its ProcessUnit components
         * @return true if server started successfully, false on failure
         * 
         * This method:
         * 1. Initializes all ProcessUnits in correct dependency order
         * 2. Enters main event loop monitoring component status
         * 3. Handles exceptions and ensures graceful shutdown on errors
         */
        [[nodiscard]] bool start();

        /**
         * @brief Stops all server components in reverse dependency order
         * 
         * Ensures clean shutdown by:
         * - Stopping input components first to prevent new messages
         * - Then stopping processing components
         * - Finally stopping output components
         */
        void stop();

    protected:
        /**
         * @brief Internal initialization of all ProcessUnit components
         * @return false on success (counterintuitive but matches implementation)
         * 
         * Starts components in dependency order:
         * 1. Output handlers (TCP/UDP)
         * 2. User session handlers (Logon/Logout/HeartBeat)
         * 3. Market containers
         * 4. Message router
         * 5. Input handler (TCP)
         */
        bool internal_start();

        /**
         * @brief Initializes market containers from configuration
         * 
         * Creates ProcessUnit containers for each configured market symbol
         * and registers them with the router for message distribution
         */
        void market_init();

    private:
        /// Flag indicating if server is currently running
        bool m_running = false;

        /// Map of market symbols to their processing containers
        std::map<std::string, ProcessUnit<pu::MarketContainer>> m_markets;

        /// Network output handler for TCP responses to clients
        ProcessUnit<pu::TcpOutputNetwork> m_tcp_output;
        
        /// Network output handler for UDP market data broadcast
        ProcessUnit<pu::UdpOutputNetwork> m_udp_output;

        /// Handler for FIX Logon messages and session establishment
        ProcessUnit<pu::user::LogonHandler> m_logon;
        
        /// Handler for FIX Logout messages and session termination
        ProcessUnit<pu::user::LogoutHandler> m_logout;
        
        /// Handler for FIX HeartBeat messages and connection monitoring
        ProcessUnit<pu::user::HeartBeatHandler> m_heartbeat;

        /// Central message router distributing messages to appropriate handlers
        ProcessUnit<pu::Router> m_router;
        
        /// Network input handler accepting TCP connections and receiving messages
        ProcessUnit<pu::TCPInputNetwork> m_tcp_input;

        /// Logger instance for server diagnostics and debugging
        std::unique_ptr<logger::ILogger> Logger = nullptr;
};