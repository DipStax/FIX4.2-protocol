#include "Common/Container/AProcessUnitBase.hpp"

#include "Common/Log/Manager.hpp"

namespace pu
{
    AProcessUnitBase::AProcessUnitBase(const std::string &_name)
        : m_name(_name), Logger(logger::Manager::newLogger(getProcessName()))
    {
    }

    const std::string &AProcessUnitBase::getProcessName() const
    {
        return m_name;
    }
}