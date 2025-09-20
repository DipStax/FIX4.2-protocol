# PgSQL

## Define

| Name | Description | Type | Default |
|---|---|---|---|
| SQL_POOL_NBCON | Default number of connection in a connection pool | `uint` | 1 |

## Description

A lightweight **PostgreSQL connection pool** for C++ built on top of **libpqxx**.
It provides efficient and thread-safe management of PostgreSQL connections, avoiding the overhead of repeatedly creating and closing connections.

## Usage

### 1. Initialize a connection pool

Each pool is identified by a **PoolId** (a `constexpr size_t`).
You must initialize the pool before using it by providing:
- A PostgreSQL connection string.
- Optionally, the number of connections to create at startup (defaults to `SQL_POOL_NBCON`).

```cpp
namespace db {
    constexpr const size_t User = 1;
}

// Initialize a pool with 5 connections
sql::ConnectionPool<db::User>::Init(
    "dbname=mydb user=admin password=secret host=localhost port=5432",
    5
);
```

### 2. Acquire a connection

`GetAvailableConnection()` will **block** until a connection is available.
Once you’re done, you **must call** `conn.done()` to return it to the pool.
If you forget, the pool will never be notified that the connection is free, even if it goes out of scope.

```cpp
sql::Connection &conn = sql::ConnectionPool<db::User>::GetAvailableConnection();
pqxx::work txn{*(conn.Conn)};

// Perform queries
auto r = txn.exec("SELECT version()");

// Commit transaction
txn.commit();

// Release the connection back to the pool
conn.done();
```

#### Thread Safe

- The pool manages connections in a thread-safe manner.
- `GetAvailableConnection()` guarantees exclusive access to a connection.
- Connections are only made available again after explicitly calling `done()`.

#### Note

- If no connections are available, `GetAvailableConnection()` will wait indefinitely until one is released.
- Multiple pools can coexist in the same application by using different template `PoolId`.
- `AddNewConnection()` allows dynamically increasing the pool size at runtime.