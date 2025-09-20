#include "Shared/PgSQL/ConnectionPool.hpp"

namespace sql
{
    template<size_t PoolId>
    void ConnectionPool<PoolId>::Init(const std::string &_connstr, size_t _size)
    {
        m_connstr = _connstr;
        for (size_t it = 0; it < _size; it++)
            AddNewConnection();
    }

    template<size_t PoolId>
    void ConnectionPool<PoolId>::AddNewConnection()
    {
        m_connections.emplace_back(std::make_unique<Connection>(m_connstr));
    }

    template<size_t PoolId>
    Connection &ConnectionPool<PoolId>::GetAvailableConnection()
    {
        while (true) {
            for (const std::unique_ptr<Connection> &_conn : m_connections) {
                std::lock_guard<std::mutex> lock(_conn->m_mutex);

                if (!_conn->m_inuse) {
                    _conn->m_inuse = true;
                    return *_conn;
                }
            }
        }
    }
}