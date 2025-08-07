#include "Client/Initiator/Shell/Builder.hpp"

namespace shell
{
    Builder &Builder::newShellCommand(const std::string &_cmd)
    {
        // m_cmd = Command{_cmd};
        return *this;
    }

    Builder &Builder::setEnvironement(const std::string &_key, const std::string &_value)
    {
        m_cmd.addEnvironement(_key, _value);
        return *this;
    }

    Builder &Builder::addArgument(const std::string &_arg)
    {
        m_cmd.addArgument(_arg);
        return *this;
    }

    Command Builder::result() const
    {
        return std::move(m_cmd);
    }
}