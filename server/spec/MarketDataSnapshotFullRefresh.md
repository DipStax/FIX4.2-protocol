# [Market Data - Snapshot/Full Refresh](https://www.onixs.biz/fix-dictionary/4.2/msgType_W_87.html)

| Tag | Field Name | Required | Comments |
|---|---|---|---|
| 262 | [MD Required ID](https://www.onixs.biz/fix-dictionary/4.2/tagNum_262.html) | `false` | Identifier of the request, required for [Market Data Request](./MarketDataRequest.md) reply |
| 55 | [Symbol](https://www.onixs.biz/fix-dictionary/4.2/tagNum_55.html) | `true` |  |
| 268 | [No DM Entries](https://www.onixs.biz/fix-dictionary/4.2/tagNum_268.html) | `true` |  |
| => 269 | [MD Entry Type](https://www.onixs.biz/fix-dictionary/4.2/tagNum_269.html) | `true` |  |
| => 270 | [MD Entry Price](https://www.onixs.biz/fix-dictionary/4.2/tagNum_270.html) | `true` |  |
| => 271 | [MD Entry Size](https://www.onixs.biz/fix-dictionary/4.2/tagNum_271.html) | `false` | Number of shares in the entry |
| => 274 | [Tick Direction](https://www.onixs.biz/fix-dictionary/4.2/tagNum_274.html) | `false` |  |