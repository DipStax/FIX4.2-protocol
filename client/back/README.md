# Backend

The **Backend** is the core processing engine of the FIX 4.2 protocol client.
It is responsible for managing the FIX session with the server, handling order management, processing market data, and maintaining the state of the trading session.
The Backend operates as a subprocess, launched and coordinated by the Initiator, and communicates with both the server and the Frontend (GUI) to provide real-time trading functionality.

---

## Overview

- **FIX Session Management:**
  The Backend establishes and maintains a FIX protocol session with the server, handling logon, logout, heartbeat, and sequence number management according to the FIX 4.2 standard.

- **Order Management:**
  It receives order requests from the Frontend, translates them into FIX messages, and sends them to the server.
  The Backend tracks the status of each order, processes execution reports, and updates the Frontend with real-time order and trade information.

- **Session State & Synchronization:**
  The Backend maintains the current session state, including open orders, positions, and message sequence numbers.
  It synchronizes with the Frontend using a secure token and API key, as coordinated by the Initiator.

- **Inter-Process Communication:**
  Communication with the Frontend is handled via IPC (Inter-Process Communication), using messages defined in the shared IPC protocol.
  The Backend listens for commands (such as new order requests or cancel requests) and responds with status updates, execution reports, and market data.

---

## Startup

The Backend is not started directly by the user.
Instead, it is launched as a subprocess by the Initiator after successful Frontend authentication.
The Initiator provides the Backend with necessary configuration, including:

- The allowed API key for session validation
- The Unix socket address for IPC communication with the Initiator
- The session token for secure synchronization with the Backend

The **Backend** is the core processing engine of the FIX 4.2 protocol client.