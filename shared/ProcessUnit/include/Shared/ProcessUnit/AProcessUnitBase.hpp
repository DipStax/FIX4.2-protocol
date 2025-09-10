#pragma once

#include <stop_token>
#include <memory>

#include "Shared/Log/ILogger.hpp"

namespace pu
{
    class AProcessUnitBase
    {
        public:
            AProcessUnitBase(const std::string &_name);
            virtual ~AProcessUnitBase() = default;

            const std::string &getProcessName() const;

        protected:
            virtual void runtime(std::stop_token _st) = 0;

            std::unique_ptr<logger::ILogger> Logger = nullptr;

        private:
            const std::string m_name;
    };
}