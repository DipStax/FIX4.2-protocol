#pragma once

#include "Client/Initiator/ProcessUnit/FrontHandler.hpp"

#include "Shared/ProcessUnit/ProcessUnit.hpp"
#include "Shared/Log/ILogger.hpp"

class Core
{
    public:
        Core();
        ~Core();

        bool start();
        void stop();

    private:

        bool m_running = false;

        ProcessUnit<pu::FrontHandler> m_front_handler;
        // ProcessUnit<pu::BackHandler> m_back_handler;

        std::unique_ptr<logger::ILogger> Logger = nullptr;
};