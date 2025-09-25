#pragma once

#include <condition_variable>

#include "Shared/ProcessUnit/AProcessUnit.hpp"

namespace pu
{
    template<class T>
    class AInputProcess : public AProcessUnit<T>
    {
        public:
            AInputProcess(const std::string &_name);
            virtual ~AInputProcess() = default;

            AProcessUnit<T>::QueueInputType &getInput() final;

        protected:
            void runtime(std::stop_token _st) final;

            virtual void onInput(AProcessUnit<T>::InputType _input) = 0;

        private:
            AProcessUnit<T>::QueueInputType m_input{};

            std::mutex m_mutex;
            std::condition_variable m_cv;
    };
}

#include "Shared/ProcessUnit/AInputProcess.inl"