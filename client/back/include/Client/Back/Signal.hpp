#pragma once

#include <atomic>

extern std::atomic<bool> signal_run;

void setup_signal_handler();

void signal_handler(int _signal);