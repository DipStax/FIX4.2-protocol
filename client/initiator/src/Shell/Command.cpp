#include <ostream>

#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "Client/Initiator/Shell/Command.hpp"

#include "Shared/Utils/Utils.hpp"

namespace shell
{
    Command::~Command()
    {
        stop();
    }

    bool Command::run()
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
            env.push_back(nullptr);
            args.push_back(strdup(m_cmd.c_str()));
            for (const std::string &_arg : m_arg) {
                char *cstr = strdup(_arg.c_str());

                args.push_back(cstr);
            }
            args.push_back(nullptr);

            execvpe(m_cmd.c_str(), args.data(), env.data());

            for (char *_env : env)
                free(_env);
            for (char *_arg : args)
                free(_arg);
            exit(1);
        } else if (m_pid > 0) {
            m_launch = true;
            return true;
        } else {
            return false;
        }
    }

    int Command::stop()
    {
        if (m_pid != -1)
        {
            kill(m_pid, SIGTERM);
            (void)status(true);
        }
        return m_exitcode;
    }

    pid_t Command::getPID() const
    {
        return m_pid;
    }

    Command::Status Command::status(bool _block)
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

    int Command::getExitCode() const
    {
        return m_exitcode;
    }

    std::ostream &operator<<(std::ostream &_os, const Command &_cmd)
    {
        _os << "Command: \"" << _cmd.m_cmd;
        for (const std::string &_arg : _cmd.m_arg)
            _os << " " << _arg;
        _os << "\"";
        for (const auto &[_key, _value] : _cmd.m_env)
            _os << "\n\tENV: " << _key << "=" << utils::trunc(_value);
        return _os;
    }

    Command::Command(const std::string &_cmd)
        : m_cmd(_cmd)
    {
    }

    void Command::addEnvironement(const std::string &_key, const std::string &_value)
    {
        m_env.emplace(_key, _value);
    }

    void Command::addArgument(const std::string &_arg)
    {
        m_arg.push_back(_arg);
    }

}