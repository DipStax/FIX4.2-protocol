# Server message specification

## Header

| Tag | Field Name | Required | Comments |
|---|---|---|---|
| 8 | [Begin String](https://www.onixs.biz/fix-dictionary/4.2/tagNum_8.html) | `true` | Always with value: **FIX.4.2**, Unencrypted, must be 1st field of the message |
| 9 | [Body Length](https://www.onixs.biz/fix-dictionary/4.2/tagNum_9.html) | `true` | Must be 2nd field of the message |
| 35 | [Message Type](https://www.onixs.biz/fix-dictionary/4.2/tagNum_35.html) | `true` | Must be 3rd field of the message |
| 49 | [Sender Id](https://www.onixs.biz/fix-dictionary/4.2/tagNum_49.html) | `true` | |
| 56 | [Target Id](https://www.onixs.biz/fix-dictionary/4.2/tagNum_56.html) | `true` | |
| 34 | [Message Sequence Number](https://www.onixs.biz/fix-dictionary/4.2/tagNum_34.html) | `true` | |
| 52 | [Sending Time](https://www.onixs.biz/fix-dictionary/4.2/tagNum_52.html) | `true` | |

## Supported message

- `0` [Heart Beat](./HeartBeat.md)
- `1` [Test Request](./TestRequest.md)
- `3` [Reject](./Reject.md)
- `5` [Logout](./Logout.md)
- `8` [Execution Report](./ExecutionReport.md)
- `j` [Business Message Reject](./BusinessMessageReject.md)
- `A` [Logon](./Logon.md)
- `D` [New Order Single](./NewOrderSingle.md)