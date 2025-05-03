#pragma once

#include <future>
#include <thread>

enum PUStatus
{
    Initialize,
    Running,
    Stop,
    Unknown
};

template<class T>
class ProcessUnit : public T
{
    public:
        template<class ...Ts>
        ProcessUnit(const std::string &_name, Ts &&..._args);
        ProcessUnit();

        void start();

        PUStatus status(float _to = 1.f);

        void stop();

        [[nodiscard]] const std::string &getName() const;

    protected:
        void process(std::stop_token _st);

    private:
        PUStatus m_status;

        std::string m_name;
        std::promise<void> m_promise;
        std::future<void> m_future;
        std::jthread m_thread;
};

#include "Server/Core/ProcessUnit/ProcessUnit.inl"