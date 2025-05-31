#pragma once

#include <chrono>
#include <string>
#include <memory>

#include "Common/Log/ILogger.hpp"

class User
{
    public:
        static User &Instance();

        [[nodiscard]] const std::string &getUserId() const;

        void setSeqNumber(size_t _seqnum);
        [[nodiscard]] size_t nextSeqNumber();
        [[nodiscard]] size_t getSeqNumber() const;

        [[nodiscard]] bool isLogin() const;

        [[nodiscard]] std::chrono::system_clock::time_point getSinceHeartBeat() const;
        void setSinceHeartBeat(std::chrono::system_clock::time_point _time);

    private:
        User();

        const std::string m_name;

        std::chrono::system_clock::time_point m_since_heartbeat = std::chrono::system_clock::now();

        std::string m_userid;
        size_t m_seqnum = 1;

        bool m_login = false;

        std::unique_ptr<log::ILogger> Logger = nullptr;
};