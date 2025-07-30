# Network

## Define

| Name | Description | Type | Default |
|---|---|---|---|
| MAX_EVENT_EPOLL | Number of event handle at once the underliyng `epoll` | `uint` | 30 |
| MAX_SOCKET | Maximum number of socket in the waiting queue of the `Acceptor` | `uint` | 30 |

## Socket

### Design

The network library provides a flexible and type-safe abstraction over sockets, supporting both Unix domain and INet (TCP/UDP) sockets.
Sockets are constructed using C++ templates, allowing you to specify the socket type (stream or datagram) and the domain (INET or UNIX) at compile time. This approach enables the use of domain-specific features and compile-time checks for correct usage.

### Declaration

The network library uses C++ templates to let you declare sockets with precise domain and type information at compile time.

#### Available Socket Types

- **Stream Sockets** (connection-oriented, e.g., TCP):
  ```cpp
  using INetTcp = net::type::Stream<net::dom::INet>;    // TCP over IPv4
  using UnixStream = net::type::Stream<net::dom::Unix>; // Unix domain stream socket
  ```

- **Datagram Sockets** (connectionless, e.g., UDP):
  ```cpp
  using INetUdp = net::type::DGram<net::dom::INet>;     // UDP over IPv4
  using UnixDGram = net::type::DGram<net::dom::Unix>;   // Unix domain datagram socket
  ```


You can also extend the `net::type` by compling with the `IsSocketType` **concept** and having a empty constructor and a other constructor taking a file descriptor as an `int`. To extend the `net::dom` you can use this template:

```cpp
namespace net::dom
{
    class Extra : public BaseSocket
    {
        public:
            static constexpr const uint32_t Domain = SOCKET_DOMAIN; // Dummy
            virtual ~Extra();

            // handle connection

        protected:
            Extra(int _type, int _proto);
            Extra(int _fd);
    };
}
```

> **Note:** Not all features are fully constrained by concepts yet (e.g., constructor requirements, `Domain` access).

These socket types are used as template parameters for `net::Acceptor` and `net::Selector`, enabling domain- and type-specific features and compile-time safety.

## Acceptor and Selector

### Acceptor

To declare an acceptor, provide the type of socket you**Unix** and **INet** require different parameters to listen and handle the underlying socket.
Using C++ concepts on the `listen` function, `net::Acceptor` determines at compile time which parameter should be used:

```cpp
auto inet_acceptor = net::Acceptor<net::type::Stream<net::dom::INet>>();
auto unix_acceptor = net::Acceptor<net::type::Stream<net::dom::Unix>>();
```

**Unix** and **INet** doesn't require the same parameter to listen and handle the underlying socket. Using `requires` concept on the listen function, the `net::Acceptor` is able at compile time to know wich parameter should be used.

```cpp
inet_acceptor.listen(8080);         // Listen on TCP port 8080

unix_acceptor.list("unix.socket");  // Listen on a Unix domain socket
unix_acceptor.unlink();             // link the Unix socket file when done
```

To accept inconming connection just call the function `net::Acceptor::accept`. By default it will be blocking but as all clas inheriting `net::BaseSocket` you can use `net::BaseSocket::setBlocking` to set it as non blocking. When the socket is non-blocking, when no client try to initate a connection the `accept` function return a `nullptr`.

```cpp
inet_acceptor.setBlocking(false);
std::shared_ptr<net::type::Stream<net::dom::INet> socket = inet_acceptor.accept();
// verify if the socket is nullptr...
```

### Selector

#### Core features

`net::Selector` are declare using the same template parameter specification, as `net::Acceptor` where the template parameter describe the socket used.
Declare a selector for the same socket type:

```cpp
net::Selector<net::type::Stream<net::dom::INet>> selector{};
selector.timeout(5.0f); // Set a timeout in seconds for event polling
```

To pull socket receiving event (data received or disconnection) use the `pull` member function:

```cpp
std::vector<std::shared_ptr<net::type::Stream<net::dom::INet>>> clients = selector.pull();
```

#### Adding/Removing client

Adding and removing client is prelly simple, use the `net::Selector` member function `client` to add a client to the selector and `erase` to remove a client from the selector. Both take a `std::shared_ptr` of the socket type as parameter.

```cpp
selector.client(socket);    // Add a client
selector.erase(socket);     // Remove a client
```

## Example

Certainly! Here is a **full example** demonstrating how to use your network library to create a simple TCP server that accepts multiple clients, uses a selector to monitor them, and echoes back any received messages. This example covers declaration, acceptor, selector, adding/removing clients, and basic data handling.

```cpp
int main() {
    // 1. Declare an acceptor for TCP (INET) sockets
    net::Acceptor<net::type::Stream<net::dom::INet>> acceptor;

    // 2. Start listening on port 5000
    if (!acceptor.listen(5000)) {
        std::cerr << "Failed to listen on port 5000" << std::endl;
        return 1;
    }
    std::cout << "Server listening on port 5000..." << std::endl;

    // 3. Create a selector for the same socket type
    net::Selector<net::type::Stream<net::dom::INet>> selector;
    selector.timeout(1.0f); // 1 second timeout for event polling

    // 4. Set the acceptor to non-blocking mode
    acceptor.setBlocking(false);

    while (true) {
        // 5. Accept new clients (non-blocking)
        auto client = acceptor.accept();
        if (client) {
            std::cout << "New client connected!" << std::endl;
            selector.client(client); // Add client to selector
        }

        // 6. Poll for events (data received or disconnection)
        auto readyClients = selector.pull();
        for (auto& c : readyClients) {
            int error = 0;
            auto data = c->receive(1024, error); // Receive up to 1024 bytes
            if (error > 0 && !data.empty()) {
                std::string msg(reinterpret_cast<const char*>(data.data()), data.size());
                std::cout << "Received: " << msg << std::endl;

                // Echo the message back to the client
                c->send(data);
            } else if (error < 0) {
                std::cout << "Client disconnected or error occurred." << std::endl;
                selector.erase(c); // Remove client from selector
            }
        }
    }
    return 0;
}
```