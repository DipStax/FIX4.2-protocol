# IPC

## Header

| Field | Descirption | Size |
|---|---|---|
| Message type | Declare the type of message | uint8 |
| Body size | Size of the body content | uint 32 |

## Message type

> `string` is declare as 4 `bytes` plus one byte per caracter in the string.

### Status:

- Message code: **0**
- size: 1 * `byte`
- Way: `back` to `front`

Notify the status of the backend during startup.

### Logon:

- Message code: **1**
- Size: 4 * `byte` + `string`
- Way: `two way`

`front` to `back`: notify the backend to connect to the server with the information provided.

`back` to `front`: inform the frontend that the client backend is correctly connected to the server. The data return is the one used by the server.

### Single Order:

- Message code: **2**
- Size: 9 * `byte` + 2 * `string`
- Way: `front` to `back`

Notify the backend to create a order with the provided information.

### New execution:

- Message code: **3**
- Size: 17 * `byte` + 3 * `string`
- Way: `back` to `front`

Event notify after a `Single Order` message. This message contain the information about an aknowledge order placed on the server.

### Execution event:

- Message code: **4**
- Size: 17 * `byte` + 3 * `string`
- Way: `back` to `front`

Message transmitted when an event on a placed order is emited by the server.