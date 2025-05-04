# FIX4.2 Protocol

A C++ implementation of the FIX 4.2 protocol, designed to simulate a basic financial trading system with client-server architecture. This project was developed as part of an Epitech educational assignment and includes components for order management, market data handling, and user interaction via a command-line interface.

## Development Status 🚧

⚠️ Note: The client component is currently not buildable due to unresolved dependencies and structural issues. Work is in progress to address these problems.

---

🔧 In parallel, the server is undergoing a major refactor to improve architectural stability, modularity, and long-term maintainability. This effort aims to create a more robust backend foundation before reintroducing a fully functional client.


## Features:

- **Client-Server Architecture**: Facilitates communication between clients and a central server using the FIX 4.2 protocol.
- **Command-Line Interface (CLI)**: Allows users to interact with the system through predefined commands.
- **Market Data Simulation**: Supports subscribing to market data feeds and refreshing market information.
- **Order Management**: Enables users to log in, log out, and manage their trading status.
- **Docker Support**: Includes Docker configurations for easy deployment and testing.
- **Modular Codebase**: Structured into distinct modules for client, server, common utilities, scripts, and tests.

## Project Structure:

```
FIX4.2-protocol/
├── client/             # Client-side application code
├── server/             # Server-side application code
├── common/             # Shared utilities and definitions
├── test/               # Unit and integration tests
├── schema.drawio       # Architectural diagrams
├── Dockerfile          # Docker configuration for containerization
├── docker-compose.yml  # Docker Compose setup for multi-container deployment
├── CMakeLists.txt      # Build configuration using CMake
├── Doxyfile            # Configuration for Doxygen documentation generation
└── README.md           # Project documentation (this file)
```
## Getting Started

### Prerequisites

- C++20 compatible compiler
- CMake 3.10 or higher
- Docker and Docker Compose (for containerized deployement)

### Building the project

1. Clone the Repository:

```bash
git clone https://github.com/DipStax/FIX4.2-protocol.git
cd FIX4.2-protocol
```

2. Build with CMake:

```bash
mkdir build
cd build
cmake ..
make
```

### Running with Docker

1. Build and start Containers:

```bash
docker-compose up --build
```

2. Access the CLI:

Once the containers are running, you can interact with the client CLI to send commands to the server.

## CLI Commands:

The client application supports the following commands:

- **logon**

```
logon -u <UserId>
```
Send a request to logon with the **UserId** passed in argument.

- **logout**
```
logout
```
Send a request to logout of the actual connected account, nothing is done if the user isn't connected.

- **status**
```
status
```
Displays the current session status and user information.

- **Refresh Market Data**
```
fresh -m <Market> -s <Side> -d <Depth> -i <Id>
```
Request a refresh on **Market** chosing the **Side** (both/ask/bid), the **Depth** (full/top/N) and providing an **Id**

- **Subscribe to Market Data**
```
sub -m <Market> -s <Side> -d <Depth> -i <Id>
```
Request a refresh on **Market** chosing the **Side** (both/ask/bid), the **Depth** (full/top/N) and providing an **Id**

- **Subcribe to Marker Data**
```
unsub -m <Market> -s <Side> -d <Depth> -i <Id>
```
Request a refresh on **Market** chosing the **Side** (both/ask/bid), the **Depth** (full/top/N) and the target **Id**

## License

This project is licensed under the [MIT License](LICENSE)

---

_Note: This project is a forl of [gabirel1/Epitech-Principe-financier-de-base-et-architecture](https://github.com/gabirel1/Epitech-Principe-financier-de-base-et-architecture) and has been adapted for educational purposes_

---