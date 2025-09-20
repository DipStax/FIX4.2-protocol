#pragma once

#include "Shared/PgSQL/Connection.hpp"

#ifndef SQL_POOL_NBCON
    #define SQL_POOL_NBCON 1
#endif

namespace sql
{
    template<size_t PoolId>
    class ConnectionPool
    {
        public:
            static constexpr const size_t Id = PoolId;

            static void Init(const std::string &_connstr, size_t _size = SQL_POOL_NBCON);
            static void AddNewConnection();

            [[nodiscard]] static Connection &GetAvailableConnection();

        private:
            ConnectionPool();

            inline static std::string m_connstr{};

            inline static std::mutex m_mutex{};
            inline static std::vector<std::unique_ptr<Connection>> m_connections{};
    };
}

#include "Shared/PgSQL/ConnectionPool.inl"