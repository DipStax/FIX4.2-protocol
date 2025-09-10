# Initiator

The **Initiator** acts as the central authentication and session management component in the client architecture.
It coordinates the secure connection and synchronization between the Frontend (GUI) and Backend components, ensuring that only authenticated users can establish a session and interact with the trading system.

## Overview

- **Session Management:**
  The Initiator manages in-memory sessions for each user login, tracking the state and credentials of both the Frontend and Backend components.

- **Authentication Coordination:**
  It validates API keys and identification messages from the Frontend, and ensures that both the Frontend and Backend are properly authenticated before allowing communication between them.

- **Process Launching:**
  Upon successful Frontend authentication, the Initiator launches the Backend as a subprocess, passing necessary session information for further authentication.

- **Token Generation & Synchronization:**
  After both components are authenticated, the Initiator generates a unique session token and distributes it to synchronize the Frontend and Backend.

## Configuration

The configuration is a `json` file provided as the first (and only) program argument. It is composed of two sections:

- `front`:
   - `port`: Port on which the **Frontend** will connect, using TCP/IP, to the **Initiator**.
- `back`:
   - `exec-path`: Path to the **backend** executable, relative to the **Initiator** directory.
   - `api-key`: Allowed API key.
   - `address`: Unix socket address on which the **backend** will connect.

#### Example

```json
{
    "front": {
        "port": 8080
    },
    "back": {
        "exec-path": "../back/Fix_Client_Back",
        "api-key": "api-key",
        "address": "/tmp/fix.initiator.socket"
    }
}
```

## Authentication protocol

### Sequence diagram

<div align="center">
  <img src="./asset/auth-protocol.png" alt="Sequence Diagram" />
  <br/>
  <hr style="width:60%;margin:auto;"/>
  <sub>Figure 1: UML Sequence Diagram of the login flow</sub>
  <br/>
  <sub>available in <a href="../../schema.drawio">schema.drawio</a></sub>
</div>

### Step by Step

1. **User Login (Frontend GUI):**
   On the GUI splash screen, when the user submits their credentials (`OnSubmit`), the authentication process begins.

2. **Frontend Connects to Initiator:**
   The **Frontend** establishes a TCP/IP connection to the **Initiator** at a port specified in the configuration.

3. **Frontend Sends Identification:**
   The **Frontend** sends an identification message to the **Initiator** ([`FrontToInitiatorAuth`](../shared/IPC/README.md#L16)), including the API Key from the configuration.

4. **Initiator Creates Session:**
   Upon receiving the identification message, the **Initiator** creates an in-memory session to manage and synchronize the connection between the **Frontend** and the **Backend**.

5. **Initiator Validate Frontend Identification:**
   The **Initiator** validates the identification and API Key received from the **Frontend**. If the credentials are valid, the **Initiator** sends an authentication validation message ([`InitiatorToFrontAuth`](../shared/IPC/README.md#L28)) back to the **Frontend** to confirm successful authentication.

6. **Initiator Launches Backend:**
   The **Initiator** starts a subprocess, launching the **Backend** executable to begin its own authentication and synchronization with the **Frontend**.

7. **Backend Connects to Initiator:**
     The **Frontend** establishes a connection to the **Initiator** using unix socket at an address provided by the **Initiator**.

8. **Backend Authenticates with Initiator:**
   The **Backend** sends an authentication message to the **Initiator** ([``](../shared/IPC/README.md#L40)), using the same API Key as the associated **Frontend**.

9. **Session Validation and Token Generation:**
   Once both the **Frontend** and **Backend** are authenticated by the **Initiator**, the session is updated with a unique token to synchronize both components ([`InitiatorToBackAuth`](../shared/IPC/README.md#L53)).

10. **Frontend Receives Backend Information:**
   The **Frontend** receives a message  from the **Initiator** ([`InitiatorToFrontValidToken`](../shared/IPC/README.md#L70)), containing the port and token required to connect to the **Backend**.

11. **Frontend Connects to Backend:**
   The **Frontend** establishes a TCP/IP connection to the **Backend** using the provided port, and sends the token and API Key for validation ([`FrontToBackValidToken`](../shared/IPC/README.md#L83)).

12. **Backend Validates Token:**
    The **Backend** checks the received token and API Key. If valid, it responds with the same token to confirm synchronization ([`BackToFrontValidToken`](../shared/IPC/README.md#L96)).

13. **Authentication Complete:**
    At this point, the **Frontend** and **Backend** are securely connected and synchronized, and the user session is fully established.