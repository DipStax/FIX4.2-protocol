#pragma once

#include <map>
#include <vector>
#include <string>

namespace shell
{
    class Command
    {
        public:
            enum Status
            {
                Running,
                SignalError,
                Runned,
                Unknown
            };

            ~Command();

            bool run();

            int stop();

            [[nodiscard]] pid_t getPID() const;
            [[nodiscard]] Status status(bool _block = false);
            [[nodiscard]] int getExitCode() const;

            friend std::ostream &operator<<(std::ostream &_os, const Command &_cmd);

        protected:
            friend class Builder;

            Command(const std::string &_cmd);

            void addEnvironement(const std::string &_key, const std::string &_value);
            void addArgument(const std::string &_arg);

        private:
            const std::string m_cmd{};
            std::map<std::string, std::string> m_env{};
            std::vector<std::string> m_arg{};

            int m_exitcode = -1;
            pid_t m_pid = -1;
            bool m_launch = false;
            Status m_status = Status::Unknown;
    };
}