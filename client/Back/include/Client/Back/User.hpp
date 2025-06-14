#pragma once

#include <chrono>
#include <string>
#include <memory>

#include "Common/Log/ILogger.hpp"

class User
{
    public:
        static User &Instance();

        void setSeqNumber(size_t _seqnum);
        void nextSeqNumber();
        [[nodiscard]] size_t getSeqNumber() const;

        void login(const std::string &_userid);
        [[nodiscard]] bool isLogin() const;
        [[nodiscard]] const std::string &getUserId() const;
        void setUserId(const std::string &_userid);

        void logoutRequested(bool _logout);
        [[nodiscard]] bool logoutRequested() const;

        void shouldDisconnect(bool _dc);
        [[nodiscard]] bool shouldDisconnect() const;
        void disconnect();

        [[nodiscard]] std::chrono::system_clock::time_point getSinceHeartBeat() const;
        void setSinceHeartBeat(std::chrono::system_clock::time_point _time);

    private:
        User();

        std::chrono::system_clock::time_point m_since_heartbeat = std::chrono::system_clock::now();

        size_t m_seqnum = 1;

        bool m_login = false;
        std::string m_userid;

        bool m_dc_req = false;
        bool m_logout_req = false;

        std::unique_ptr<logger::ILogger> Logger = nullptr;
};