#include "Shared/ProcessUnit/AInputProcess.hpp"

namespace pu
{
    template<class T>
    AInputProcess<T>::AInputProcess(const std::string &_name)
        : AProcessUnit<T>(_name)
    {
    }

    template<class T>
    AProcessUnit<T>::QueueInputType &AInputProcess<T>::getInput()
    {
        return m_input;
    }

    template<class T>
    void AInputProcess<T>::runtime(std::stop_token _st)
    {
        this->Logger->template log<logger::Level::Info>("Launching process unit runtime");

        while (!_st.stop_requested()) {
            std::unique_lock<std::mutex> lock(m_mutex);

            m_cv.wait_for(lock, std::chrono::milliseconds(100), [this, _st] {
                return !m_input.empty() || _st.stop_requested();
            });

            while (!m_input.empty()) {
                this->Logger->template log<logger::Level::Debug>("Receive new input");
                onInput(m_input.pop_front());
            }
        }
        this->Logger->template log<logger::Level::Info>("Exiting process unit runtime");
    }
}