#pragma once

#include <chrono>
#include <string>
#include <memory>

#include "Shared/Log/ILogger.hpp"

class User
{
    public:
        static User &Instance();

        struct HeartBeatInfo
        {
            std::chrono::system_clock::time_point Since{};
            float Elapsing;
        };

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

        HeartBeatInfo &getHeartBeatInfo();

    private:
        User();

        HeartBeatInfo m_hb_info;

        std::chrono::system_clock::time_point m_since_heartbeat = std::chrono::system_clock::now();

        size_t m_seqnum = 1;

        bool m_login = false;
        std::string m_userid;

        bool m_dc_req = false;
        bool m_logout_req = false;

        std::unique_ptr<logger::ILogger> Logger = nullptr;
};