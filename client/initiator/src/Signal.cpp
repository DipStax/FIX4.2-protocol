#include <iostream>
#include <csignal>

#include "Client/Initiator/Signal.hpp"

std::atomic<bool> signal_run{false};

void setup_signal_handler()
{
    std::signal(SIGTERM, signal_handler);
    std::signal(SIGINT, signal_handler);

    signal_run = true;
}

void signal_handler(int _signal) {
    std::cout << "Received signal " << _signal << ": terminating programme" << std::endl;
    signal_run = false;
}