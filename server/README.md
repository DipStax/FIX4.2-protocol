# Server

## FIX specification

You can find the specification of the supported message and their feild in the [`spec`](./spec/README.md) folder.
This provide a table as:

| Tag | Field Name | Required | Comments |
|---|---|---|---|
| 1 | Example Field | `true` | This is an example field |

- `Tag`: the tag reference by the field.
- `Field Name`: the name of the field, the link is a reference to the FIX4.2 protocol manual followed by the server (as the title).
- `Required`: specify if it is required (most of the time it is, because this **PoC**)
- `Comments`: additionnal comment on the field and what it representes.

## Implementation

### Login

You can login using the [`Logon`](./spec/Logon.md) message, if the login is valid the server will respond with a `Logon` Message that aknowledge the login. A session will be open until you send the `Logout` (see next section) or disconnect the socket.

> Currently no verification are done on the `Sender Id`, no database and no in memory verification for duplicate.

### Logout

To end a session, send a [`Logout`](./spec/Logout.md) message.
Upon receiving a valid Logout request, the server will respond with a Logout message and close the session gracefully.
If a client disconnects unexpectedly or violates protocol, the server may also initiate a Logout.
You can use Logout to ensure all session state is cleared and resources are released.

### HeartBeat/Test Request

The server expects periodic [`HeartBeat`](./spec/HeartBeat.md) messages from each connected client to confirm the connection is alive.
If the server does not receive a heartbeat within the configured timeout, it will send a [`TestRequest`](./spec/TestRequest.md) message to the client.
If the client fails to respond to the TestRequest, the server will consider the connection lost and may initiate a Logout.

> You can configure heartbeat intervals in the `Logon` message.

### Order Managment

You can submit orders using the [`New Order Single`](./spec/NewOrderSingle.md) message, targeting different markets (currently only limit orders are supported).
When an order is matched or executed, the server will send an [`Execution Report`](./spec/ExecutionReport.md) message with details about the event (status, price, etc.).
Order management includes validation, routing, and matching against the order book.

### Rejection

Their is 2 available rejection type:
- [`Buisness`](./spec/BusinessMessageReject.md): reject on message/transaction made at the buisness level (e.g., order book or trading logic errors).
- [`Standard`](./spec/Reject.md): reject message made at the system level (e.g., logon failures, invalid headers, protocol violations)

> Not all case of rejection are coverade, so if you don't receive a respond after sending a message, consider it to be rejected (the `Message Sequence Number` is not incremented for the reply)

## Configuration

### Define

This is all the compiler definition available for the server configuration.

| Name | Description | Type | Default |
|---|---|---|---|
| **PU_LOGON_TP_SIZE** | Number of thread in the threadpool of the **logon** process unit, represent the number of message that can be process simultaneously | `uint` | 1 |
| **PU_LOGON_HB_MAX_TO** | Maximum time, in second, allow between each `HeartBeat` message | `float` | 100 |
| **PU_HEARTBEAT_TP_SIZE** | Number of thread in the threadpool of the **heartbeat** process unit, represent the number of message that can be process simultaneously | `uint` | 1 |
| **PU_LOGOUT_TP_SIZE** | Number of thread in the threadpool of the **heartbeat** process unit, represent the number of message that can be process simultaneously | `uint` | 1 |
| **PU_NETOUT_TP_SIZE** | Number of thread in the threadpool of the **network output** process unit, represent the number of message that can be send simultaneously | `uint` | 30 |
