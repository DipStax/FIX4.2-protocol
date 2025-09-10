#include "Shared/Log/Imp/Buffer.hpp"
#include "Shared/Log/Manager.hpp"

namespace logger::imp
{
    Buffer::Buffer(const std::string &_name)
        : Base(_name)
    {
        m_loggers.push_back(logger::Manager::newLogger("console", _name));
        m_loggers.push_back(logger::Manager::newLogger("file", _name));
    }

    void Buffer::newEventLog(Event _event)
    {
        m_tp.enqueue([this, _event = std::move(_event)] () mutable {
            for (const std::unique_ptr<ILogger> &_logger : m_loggers)
                _logger->newEventLog(_event);
        });
    }
}