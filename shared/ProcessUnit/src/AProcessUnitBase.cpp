#include "Shared/ProcessUnit/AProcessUnitBase.hpp"

#include "Shared/Log/Manager.hpp"

namespace pu
{
    AProcessUnitBase::AProcessUnitBase(const std::string &_name)
        : Logger(logger::Manager::newLogger(_name)), m_name(_name)
    {
    }

    const std::string &AProcessUnitBase::getProcessName() const
    {
        return m_name;
    }
}