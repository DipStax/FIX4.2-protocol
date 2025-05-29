#pragma once

#include <chrono>
#include <string>
#include <memory>

#include "Common/Log/ILogger.hpp"

class User
{
    public:
        static User &Instance();

        const std::string &getName() const;

        size_t getSeqNumber() const;

        [[nodiscard]] std::chrono::system_clock::time_point getSinceHeartBeat() const;
        std::chrono::system_clock::time_point setSinceHeartBeat(const std::string &_time, const std::string &_format = "%Y%m%d-%H:%M:%S");
        void setSinceHeartBeat(std::chrono::system_clock::time_point _time);

    private:
        User();

        const std::string m_name;

        std::chrono::system_clock::time_point m_since_heartbeat = std::chrono::system_clock::now();

        size_t m_seqnum = 0;

        std::unique_ptr<log::ILogger> Logger = nullptr;
};