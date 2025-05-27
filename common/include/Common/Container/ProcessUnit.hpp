#pragma once

#include <future>
#include <thread>

#include "Common/Container/IProcessUnitStopable.hpp"

enum PUStatus
{
    Initialize,
    Running,
    Stop,
    Unknown
};

template<class T>
concept IsPUStopable = std::is_base_of_v<pu::IProcessUnitStopable, T>;

template<class T>
class ProcessUnit : public T
{
    public:
        ProcessUnit() = default;
        template<class ...Ts>
        ProcessUnit(Ts &&..._args);

        void start();

        PUStatus status(float _to = 1.f);

        void stop();

    protected:
        void process(std::stop_token _st);

    private:
        std::promise<void> m_promise;
        std::future<void> m_future;
        std::jthread m_thread;
};

#include "Common/Container/ProcessUnit.inl"