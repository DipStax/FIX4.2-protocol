## Shared (Server/Client)

Available library:
- [Container](./Container/README.md): Provides custom data structures and containers used throughout the project, such as specialized maps.
- [Core](./Container/README.md): Contains core utilities, base classes, and fundamental types that are shared across server and client components, including common interfaces and essential logic.
- [Log](./Log/README.md): Implements the logging framework, supporting multiple log levels, output backends (console, file, etc.), and structured logging for debugging and monitoring.
- [Message](./Message/README.md): Defines message formats, serialization/deserialization routines, and helpers for handling protocol messages exchanged between server and client.
- [Network](./Network/README.md): Provides networking abstractions and utilities, including socket management, and network buffer handling for reliable communication.
- [Process Unit](./ProcessUnit/README.md): Provides abstractions for concurrent and parallel task execution. Enables the creation of self-contained units of work that can be managed independently or in thread pools, simplifying the development of scalable, high-performance, and responsive application components.
- [Thread](./Thread/README.md): Supplies threading primitives, thread pools, and synchronization mechanisms to enable safe and efficient concurrent processing.
- [Utils](./Utils/README.md): Offers miscellaneous utility functions, helpers, and extensions that support various tasks such as string manipulation, and time handling.