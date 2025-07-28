# Client

## [GUI](./GUI/README.md)

This folder contains the graphical user interface for the FIX 4.2 protocol client.
It provides user-facing components for order entry, order history, and login management, allowing users to interact with the backend and manage trading activities visually.

Key features:

- Modern Qt-based GUI for the FIX client
- Order entry and execution interface
- Order history and status tracking
- User authentication and session management

## [Backend](./back/README.md)

This folder contains the backend logic for the FIX 4.2 protocol client.
It provides core components, process units, and network interfaces required for client-side order management, authentication, and communication with external systems.

Key features:

- Core backend logic for the client
- Modular process units (order routing, market execution, single user management)
- Network input/output handling

## [Shared](./shared/README.md)

Available library:
- [IPC](./shared/IPC/README.md): **I**nter-**P**rocess **C**ommunication protocol definition and build