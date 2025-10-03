# [Execution Report](https://www.onixs.biz/fix-dictionary/4.2/msgType_8_8.html)

| Tag | Field Name | Required | Comments |
|---|---|---|---|
| 37 | [Order Id](https://www.onixs.biz/fix-dictionary/4.2/tagNum_37.html) | `true` | Unique Id of the order |
| 37 | [Caller Order Id](https://www.onixs.biz/fix-dictionary/4.2/tagNum_37.html) | `false` | Unique Id of the execution reference |
| 17 | [Execution Id](https://www.onixs.biz/fix-dictionary/4.2/tagNum_17.html) | `true` | Unique Id for the `Execution Report` |
| 20 | [Execution Transaction Type](https://www.onixs.biz/fix-dictionary/4.2/tagNum_20.html) | `true` | Global report status |
| 150 | [Execution Type](https://www.onixs.biz/fix-dictionary/4.2/tagNum_150.html) | `true` | Action apply to the order |
| 150 | [Order Status](https://www.onixs.biz/fix-dictionary/4.2/tagNum_39.html) | `true` | Current state of the order |
| 103 | [Order Reject Reason](https://www.onixs.biz/fix-dictionary/4.2/tagNum_103.html) | `false` | Can be filled if the order is rejected |
| 55 | [Symbol](https://www.onixs.biz/fix-dictionary/4.2/tagNum_55.html) | `true` | |
| 54 | [Side](https://www.onixs.biz/fix-dictionary/4.2/tagNum_54.html) | `true` | Only support `1` (Buy), `2` (Sell), `3` (Buy Less) or `4` (Sell Plus) |
| 38 | [Order Quantity](https://www.onixs.biz/fix-dictionary/4.2/tagNum_38.html) | `false` | Original quantity of the order, required for now (doesn't support `CashOrderQty`) |
| 40 | [Order Type](https://www.onixs.biz/fix-dictionary/4.2/tagNum_40.html) | `false` | Only support `2` (Limit) |
| 44 | [Price](https://www.onixs.biz/fix-dictionary/4.2/tagNum_44.html) | `false` | Filled for aknowledge of a new order only if provide in the `NewOrderSingle` message |
| 151 | [Leave Quantity](https://www.onixs.biz/fix-dictionary/4.2/tagNum_151.html) | `false` | Fill for an report on an executed order. Remaining quantity on the order executed. |
| 32 | [Last Shares](https://www.onixs.biz/fix-dictionary/4.2/tagNum_32.html) | `false` | Fill for an report on an executed order. Last quantity executed on the order. |
| 31 | [Last Price](https://www.onixs.biz/fix-dictionary/4.2/tagNum_31.html) | `false` | Fill for an report on an executed order. Last price executed on the order. |
| 151 | [Leave Quantity](https://www.onixs.biz/fix-dictionary/4.2/tagNum_151.html) | `true` | Remaining quantity on the order |
| 14 | [Cumulative Quantity](https://www.onixs.biz/fix-dictionary/4.2/tagNum_14.html) | `true` | Quantity executed |
| 6 | [Averga Price](https://www.onixs.biz/fix-dictionary/4.2/tagNum_6.html) | `true` | Average price of the executed quantity |
| 58 | [Text](https://www.onixs.biz/fix-dictionary/4.2/tagNum_58.html) | `false` | |