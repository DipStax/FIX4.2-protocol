#include "Shared/ProcessUnit/AProcessUnit.hpp"

namespace pu
{
    template<class T>
    AProcessUnit<T>::AProcessUnit(const std::string &_name)
        : AProcessUnitBase(_name)
    {
    }
}