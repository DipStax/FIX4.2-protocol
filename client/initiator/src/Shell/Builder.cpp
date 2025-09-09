#include "Client/Initiator/Shell/Builder.hpp"

namespace shell
{
    Builder &Builder::newShellCommand(const std::string &_cmd)
    {
        m_cmd = std::make_unique<Command>(Command{_cmd});
        return *this;
    }

    Builder &Builder::setEnvironement(const std::string &_key, const std::string &_value)
    {
        m_cmd->addEnvironement(_key, _value);
        return *this;
    }

    Builder &Builder::addArgument(const std::string &_arg)
    {
        m_cmd->addArgument(_arg);
        return *this;
    }

    std::unique_ptr<Command> &&Builder::result()
    {
        return std::move(m_cmd);
    }
}