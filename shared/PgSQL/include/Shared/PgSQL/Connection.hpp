#pragma once

#include <pqxx/pqxx>

namespace sql
{
    struct Connection
    {
        public:
            Connection(const std::string &_connstr);

            void done();

            std::unique_ptr<pqxx::connection> Conn = nullptr;

        private:
            template<size_t Id>
            friend class ConnectionPool;

            std::mutex m_mutex{};
            bool m_inuse = false;
    };
}