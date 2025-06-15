#include "Common/Log/Manager.hpp"

namespace logger
{
    std::unique_ptr<ILogger> Manager::newLogger(const std::string &_type, const std::string &_name)
    {
        return m_custom[_type](_name);
    }

    std::unique_ptr<ILogger> Manager::newLogger(const std::string &_name)
    {
        return m_default(_name);
    }
}