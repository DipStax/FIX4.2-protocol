#pragma once

#include <future>
#include <thread>

#include "Shared/ProcessUnit/IProcessUnitStopable.hpp"
#include "Shared/ProcessUnit/AProcessUnitBase.hpp"

enum class PUStatus : uint8_t
{
    Unknown = 0,
    Initialize,
    Running,
    Stop
};

template<class T>
concept IsPUStopable = std::is_base_of_v<pu::IProcessUnitStopable, T>;

template<class T>
concept IsProcessUnitBase = std::is_base_of_v<pu::AProcessUnitBase, T>;

template<IsProcessUnitBase T>
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

#include "Shared/ProcessUnit/ProcessUnit.inl"