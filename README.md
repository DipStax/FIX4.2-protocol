# FIX4.2 Protocol

A C++ implementation of the FIX 4.2 protocol, designed to simulate a basic financial trading system with client-server architecture. This project includes components for order management, market data handling.

## Development Status

The project is currently, both the client and the server, under heavy refactor to improve architectural stability, modularity, and long-term maintainability.

## Features:

- **Client-Server Architecture**: Facilitates communication between clients and a central server using the FIX 4.2 protocol.
- **User interface (UI)**: Allows users to interact with the system through QT UI.
- **Market Data Simulation**: Supports subscribing to market data feeds and refreshing market information.
- **Order Management**: Enables users to log in, log out, and manage their trading status.
- **Modular Codebase**: Structured into distinct modules for client, server, common utilities, scripts, and tests.

## Project Structure:

## Getting Started

### Prerequisites

- C++20 compatible compiler
- CMake 3.10 or higher
- Docker and Docker Compose (for containerized deployement)
- Qt6 developement tool kit

### Building the project

1. Clone the Repository:

```bash
git clone https://github.com/DipStax/FIX4.2-protocol.git
cd FIX4.2-protocol
```

2. Build with CMake:

To build the client use set the cmake optiona `ClientBuild` to **true** and for the server use `ServerBuild`.

```bash
mkdir build
cd build
cmake -DClientBuild=TRUE -DServerBuild=TRUE ..
make
```

## License

This project is licensed under the [MIT License](LICENSE)

---

_Note: This project is a forl of [gabirel1/Epitech-Principe-financier-de-base-et-architecture](https://github.com/gabirel1/Epitech-Principe-financier-de-base-et-architecture) and has been adapted for educational purposes_

---