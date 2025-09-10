# FIX4.2 Protocol

A C++ implementation of the FIX 4.2 protocol, designed to simulate a basic financial trading system with client-server architecture. This project includes components for order management, market data handling.

## Development Status

The project is currently, both the client and the server, under heavy refactor to improve architectural stability, modularity, and long-term maintainability.

## Features:

- **Client-Server Architecture**: Facilitates communication between clients and a central server using the FIX 4.2 protocol.
- **User interface (UI)**: Allows users to interact with the system through QT UI.
- **Order Management**: Enables users to log in, log out, and manage their trading status.
- **Modular Codebase**: Structured into distinct modules for client, server, common utilities, tools, and tests.

## Project Structure:

- [Client](./client/README.md): Contains the client-side components for interacting with the server, submitting orders, and viewing market data.
    - [Backend](./client/back/README.md): Provides the core backend logic for the client, including process units, network communication, and order management.
    - [GUI](./client/GUI/README.md): Implements the graphical user interface for the client, allowing users to interact with the trading system visually.
    - [Initiator](./client/initiator/README.md): Coordinates authentication and session management between the Frontend (GUI) and Backend components.
    - [Shared](./client/shared/README.md): Defines shared types, utilities, and protocol definitions used by both the client backend and GUI.
- [Server](./server/README.md): Implements the server-side logic for the FIX 4.2 protocol, including order book management, client session handling, and message routing.
- [Shared](./shared/README.md): Offers common libraries and abstractions (such as containers, logging, networking, threading, and process units) used throughout both client and server components.

## Getting Started

### Prerequisites

- C++20 compatible compiler
- CMake 3.10 or higher
- Qt6 developement tool kit
- Doxygen (optional)
- Draw.io extension (optional)

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

_Note: This project is a fork of [gabirel1/Epitech-Principe-financier-de-base-et-architecture](https://github.com/gabirel1/Epitech-Principe-financier-de-base-et-architecture) wich has been adapted for educational purposes, but this version is a complete rewrite and aim to be a **PoC** of a server/client_

---

## Maintenair

Maintened by [@DipStax](https://github.com/DipStax)