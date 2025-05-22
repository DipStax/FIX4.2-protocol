#include "Common/Log/Imp/Buffer.hpp"
#include "Common/Log/Manager.hpp"

namespace log::imp
{
    Buffer::Buffer(const std::string &_name)
        : Base(_name)
    {
        m_loggers.push_back(log::Manager::newLogger("console", _name));
        m_loggers.push_back(log::Manager::newLogger("file", _name));

        m_thread = std::jthread(&Buffer::thread, this);
    }

    Buffer::~Buffer()
    {
        if (m_thread.joinable()) {
            m_thread.request_stop();
            m_thread.join();
        }
    }

    void Buffer::newEventLog(Event _event)
    {
        m_queue.push(_event);
    }

    void Buffer::thread(std::stop_token _st)
    {
        while (!_st.stop_requested()) {
            while (!m_queue.empty()) {
                for (const std::unique_ptr<ILogger> &_logger : m_loggers)
                    _logger->newEventLog(m_queue.front());
                m_queue.pop();
            }
        }
    }
}