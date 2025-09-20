CREATE ROLE db_admin WITH LOGIN PASSWORD 'admin';
CREATE ROLE db_app WITH LOGIN PASSWORD 'app';
CREATE ROLE db_readonly WITH LOGIN PASSWORD 'readonly';

CREATE SCHEMA dev AUTHORIZATION db_admin;

CREATE TABLE dev.client (
    id int NOT NULL PRIMARY KEY,
    apikey CHAR(36),
    client_name varchar,
    server_name varchar,
    connected BOOLEAN
);