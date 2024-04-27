#include "Common/Network/CEPoll.hpp"

namespace net::c
{
    int EPoll::create(int _max)
    {
        return epoll_create(_max);
    }

    int EPoll::wait(int _fd, Event *_event, int _maxev, int _to)
    {
        return epoll_wait(_fd, _event, _maxev, _to);
    }

    int EPoll::ctl(int _epfd, int _op, int _fd, Event *_event)
    {
        return epoll_ctl(_epfd, _op, _fd, _event);
    }

    EPoll::EPoll(int _max)
        : m_max(_max)
    {
        create();
    }

    void EPoll::create()
    {
        m_fd = create(m_max);
    }

    int EPoll::wait(Event *_events, int _to)
    {
        return wait(m_fd, _events, m_max, _to);
    }

    int EPoll::ctl(int _op, int _fd, Event *_event)
    {
        return ctl(m_fd, _op, _fd, _event);
    }
}