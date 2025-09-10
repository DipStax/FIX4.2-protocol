#include <iostream>

#include "Shared/Log/Manager.hpp"

namespace logger
{
    template<class T>
    void Manager::registerNewLogger(const std::string &_type)
    {
        m_custom[_type] = [] (const std::string &_sub) {
            return std::make_unique<T>(_sub);
        };
    }

    template<class T>
    void Manager::registerDefaultLogger()
    {
        m_default = [] (const std::string &_sub) {
            return std::make_unique<T>(_sub);
        };
    }
}