#include "Shared/PgSQL/Connection.hpp"

namespace sql
{
    Connection::Connection(const std::string &_connstr)
        : Conn(std::make_unique<pqxx::connection>(_connstr))
    {
    }

    void Connection::done()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        m_inuse = false;
    }
}