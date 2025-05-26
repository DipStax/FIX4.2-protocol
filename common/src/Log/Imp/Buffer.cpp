#include "Common/Log/Imp/Buffer.hpp"
#include "Common/Log/Manager.hpp"

namespace log::imp
{
    Buffer::Buffer(const std::string &_name)
        : Base(_name)
    {
        m_loggers.push_back(log::Manager::newLogger("console", _name));
        m_loggers.push_back(log::Manager::newLogger("file", _name));
    }

    void Buffer::newEventLog(Event _event)
    {
        m_tp.enqueue([this, _event = std::move(_event)] () mutable {
            for (const std::unique_ptr<ILogger> &_logger : m_loggers)
                _logger->newEventLog(_event);
        });
    }
}