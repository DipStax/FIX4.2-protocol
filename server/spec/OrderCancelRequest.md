# [Order Cancel Request](https://www.onixs.biz/fix-dictionary/4.2/msgType_F_70.html)

| Tag | Field Name | Required | Comments |
|---|---|---|---|
| 41 | [Original Caller Order Id](https://www.onixs.biz/fix-dictionary/4.2/tagNum_41.html) | `true` | Unique Id of the order canceled |
| 11 | [Caller Order Id](https://www.onixs.biz/fix-dictionary/4.2/tagNum_11.html) | `true` | Id related to the cancel itself |
| 55 | [Symbol](https://www.onixs.biz/fix-dictionary/4.2/tagNum_55.html) | `true` | |
| 54 | [Side](https://www.onixs.biz/fix-dictionary/4.2/tagNum_54.html) | `true` | Side on wich the order is place |
| 60 | [Transaction Time](https://www.onixs.biz/fix-dictionary/4.2/tagNum_60.html) | `true` | Time this order request was initiated |
| 38 | [Order Quantity](https://www.onixs.biz/fix-dictionary/4.2/tagNum_38.html) | `false` | Quantity of the canceled order (required because no support for `CashOrderQty`) |