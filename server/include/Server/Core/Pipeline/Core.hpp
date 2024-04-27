#pragma once

#include <concepts>
#include <future>

using ms = unsigned long long int;

/// @brief Core of any pipeline running, managing the thread and it's status
/// @tparam T is a derived from `Pipeline` use to start it. 
template<class T>
class Pipeline
{
    public:
        Pipeline();
        virtual ~Pipeline();

        /// @brief Get the status of the pipeline
        /// @param _to Timeout of the request on the status
        /// @return Return true if the pipeline is running otherwise false.
        bool status(float _to = 1.f);

        /// @brief Stop the pipeline if it's running, It throw an exception if the thread stoped because of one.
        /// @return the status of the thread after the stop.
        [[nodiscard]] std::future_status stop();

    protected:
        /// @brief Setup and start the pipeline in an other thread and the member function `loop` of the parameter.
        /// @param _pipeline Pipeline with an `loop` member function implementation.
        static void tstart(T *_pipeline);

        /// @brief Try to fetch the status of the thread, It throw the exception if the thread stoped because of one.
        /// @param _to Timeout of the status request (in millisecond) 
        /// @return The status of the thread running the pipeline.
        [[nodiscard]] std::future_status tstatus(ms _to);

        bool m_running = false;     ///< Define if the pipeline should be running.

    private:
        std::future<void> m_futur;  ///< Only relation with the running thread.
};

#include "Server/Core/Pipeline/Core.inl"