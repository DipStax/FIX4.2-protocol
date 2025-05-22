#pragma once

#include <filesystem>
#include <functional>
#include <map>
#include <memory>

#include "Common/Log/ILogger.hpp"

namespace log
{
    class Manager
    {
        public:
            Manager() = delete;

            struct Configuration
            {
                std::filesystem::path LogDir;
            };

            inline static Configuration Config{
                "./log/"
            };

            static std::unique_ptr<ILogger> newLogger(const std::string &_type, const std::string &_name);
            static std::unique_ptr<ILogger> newLogger(const std::string &_name);

            template<class T>
            static void registerNewLogger(const std::string &_type);
            template<class T>
            static void registerDefaultLogger();

        private:
            using LoggerCtor = std::function<std::unique_ptr<ILogger>(const std::string &)>;

            inline static std::map<std::string, LoggerCtor> m_custom;
            inline static LoggerCtor m_default;
    };
}

#include "Common/Log/Manager.inl"