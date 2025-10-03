# [Order Cancel Reject](https://www.onixs.biz/fix-dictionary/4.2/msgType_9_9.html)

| Tag | Field Name | Required | Comments |
|---|---|---|---|
| 41 | [Order Id](https://www.onixs.biz/fix-dictionary/4.2/tagNum_41.html) | `true` | Id related to the cancel itself |
| 11 | [Original Caller Order Id](https://www.onixs.biz/fix-dictionary/4.2/tagNum_11.html) | `true` | Unique Id of the order canceled |
| 55 | [Order Status](https://www.onixs.biz/fix-dictionary/4.2/tagNum_55.html) | `true` | Status of the order after the reject |
| 434 | [Cancel Reject Response To](https://www.onixs.biz/fix-dictionary/4.2/tagNum_434.html) | `false` | Reject repsonse to Cancel or Replace |
| 380 | [Cancel Reject Reason](https://www.onixs.biz/fix-dictionary/4.2/tagNum_380.html) | `true` | Reason of the reject |
| 58 | [Text](https://www.onixs.biz/fix-dictionary/4.2/tagNum_58.html) | `false` | Quantity of the canceled order (required because no support for `CashOrderQty`) |