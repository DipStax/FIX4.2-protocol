#pragma once

#include <memory>

#include "Client/Initiator/Shell/Command.hpp"

namespace shell
{
    class Builder
    {
        public:
            Builder() = default;
            ~Builder() = default;

            Builder &newShellCommand(const std::string &_cmd);
            Builder &setEnvironement(const std::string &_key, const std::string &_value);
            Builder &addArgument(const std::string &_arg);
            std::unique_ptr<Command> result();

        private:
            std::unique_ptr<Command> m_cmd{};
    };
}