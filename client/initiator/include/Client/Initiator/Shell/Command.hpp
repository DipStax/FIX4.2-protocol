#pragma once

#include <map>
#include <vector>
#include <string>

#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

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

            ~Command()
            {
                stop();
            }

            int stop()
            {
                if (m_pid)
                {
                    kill(m_pid, SIGTERM);
                    (void)status(true);
                }
                return m_exitcode;
            }

            bool run()
            {
                m_pid = fork();

                if (m_pid == 0) {
                    std::vector<char *> env;
                    std::vector<char *> args;

                    for (const auto& [key, value] : m_env) {
                        std::string combined = key + "=" + value;
                        char *cstr = strdup(combined.c_str());

                        env.push_back(cstr);
                    }
                    for (const std::string &_arg : m_arg) {
                        char *cstr = strdup(_arg.c_str());

                        args.push_back(cstr);
                    }

                    execvpe(m_cmd.c_str(), args.data(), env.data());

                    for (char *_env : env)
                        free(_env);
                    for (char *_arg : args)
                        free(_arg);
                    exit(0);
                } else if (m_pid > 0) {
                    m_launch = true;
                    return true;
                } else {
                    return false;
                }
            }

            [[nodiscard]] Status status(bool _block = false)
            {
                int status = 0;

                waitpid(m_pid, &status, _block ? 0 : WNOWAIT);
                if (WIFEXITED(status)) {
                    m_exitcode = WEXITSTATUS(status);
                    m_status = Status::Runned;
                } else if (WIFSIGNALED(status)) {
                    m_exitcode = WTERMSIG(status);
                    m_status = Status::SignalError;
                } else if (WIFCONTINUED(status)) {
                    m_status = Status::Running;
                }
                return m_status;
            }

            [[nodiscard]] int getExitCode() const
            {
                return m_exitcode;
            }

        protected:
            friend class Builder;
            Command() = default;
            Command(const std::string &_cmd)
                : m_cmd(_cmd)
            {
            }

            void addEnvironement(const std::string _key, const std::string &_value)
            {
                m_env.emplace(_key, _value);
            }

            void addArgument(const std::string &_arg)
            {
                m_arg.push_back(_arg);
            }

            friend std::ostream &operator<<(std::ostream &_os, const Command &_cmd)
            {
                _os << "Command: \"" << _cmd.m_cmd;
                for (const std::string &_arg : _cmd.m_arg)
                    _os << " " << _arg;
                _os << "\"";
                for (const auto &[_key, _value] : _cmd.m_env)
                    _os << "\n\tENV: " << _key << "=" << _value;
                return _os;
            }

        private:
            pid_t m_pid = -1;
            Status m_status = Status::Unknown;
            const std::string m_cmd{};
            std::map<std::string, std::string> m_env{};
            std::vector<std::string> m_arg{};
            int m_exitcode = -1;
            bool m_launch = false;
    };
}