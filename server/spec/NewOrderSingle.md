# [New Order Single](https://www.onixs.biz/fix-dictionary/4.2/msgType_D_68.html)

| Tag | Field Name | Required | Comments |
|---|---|---|---|
| 11 | [Caller Order Id](https://www.onixs.biz/fix-dictionary/4.2/tagNum_11.html) | `true` | Unique Id of the order |
| 21 | [Handle Instance](https://www.onixs.biz/fix-dictionary/4.2/tagNum_21.html) | `true` | Strategy of execution, only support `3` (Manual) |
| 55 | [Symbol](https://www.onixs.biz/fix-dictionary/4.2/tagNum_55.html) | `true` | |
| 54 | [Side](https://www.onixs.biz/fix-dictionary/4.2/tagNum_54.html) | `true` | Only support `1` (Buy), `2` (Sell), `3` (Buy Less) or `4` (Sell Plus) |
| 38 | [Order Quantity](https://www.onixs.biz/fix-dictionary/4.2/tagNum_38.html) | `false` | |
| 40 | [Order Type](https://www.onixs.biz/fix-dictionary/4.2/tagNum_40.html) | `true` | Only support `2` (Limit) |
| 44 | [Price](https://www.onixs.biz/fix-dictionary/4.2/tagNum_44.html) | `false` | Required for `Order Type` = `2` |