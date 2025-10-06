# [Market Data - Incremental Refresh](https://www.onixs.biz/fix-dictionary/4.2/msgType_V_86.html)

| Tag | Field Name | Required | Comments |
|---|---|---|---|
| 262 | [MD Required ID](https://www.onixs.biz/fix-dictionary/4.2/tagNum_262.html) | `false` | Identifier of the request, required for [Market Data Request](./MarketDataRequest.md) reply |
| 268 | [No MD Entries](https://www.onixs.biz/fix-dictionary/4.2/tagNum_268.html) | `true` |  |
| => 279 | [MD Update Action](https://www.onixs.biz/fix-dictionary/4.2/tagNum_279.html) | `true` | Action to apply |
| => 269 | [MD Entry Type](https://www.onixs.biz/fix-dictionary/4.2/tagNum_269.html) | `false` | requires for **MD Update Action** = New (`0`) |
| => 280 | [MD Entry Reference ID](https://www.onixs.biz/fix-dictionary/4.2/tagNum_280.html) | `false` | Subscription ID |
| => 55 | [Symbol](https://www.onixs.biz/fix-dictionary/4.2/tagNum_55.html) | `false` |  |
| => 270 | [MD Entry Price](https://www.onixs.biz/fix-dictionary/4.2/tagNum_270.html) | `false` |  |
| => 271 | [MD Entry Size](https://www.onixs.biz/fix-dictionary/4.2/tagNum_271.html) | `false` | Quantity of shares related to the entry |
| => 274 | [Tick Direction](https://www.onixs.biz/fix-dictionary/4.2/tagNum_274.html) | `false` |  |
