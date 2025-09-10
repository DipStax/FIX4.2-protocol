# [Reject](https://www.onixs.biz/fix-dictionary/4.2/msgType_3_3.html)

| Tag | Field Name | Required | Comments |
|---|---|---|---|
| 45 | [Reference Sequence Number](https://www.onixs.biz/fix-dictionary/4.2/tagNum_45.html) | `true` | Reference to the `Message Sequence Number` of the rejected message |
| 371 | [Reference Tag Id](https://www.onixs.biz/fix-dictionary/4.2/tagNum_371.html) | `false` | Tag of the field rejected, always fill by the server |
| 372 | [Reference Message Type](https://www.onixs.biz/fix-dictionary/4.2/tagNum_372.html) | `false` | `Message Type` of the rejected message, always fill by the server |
| 373 | [Session Reject Reason](https://www.onixs.biz/fix-dictionary/4.2/tagNum_373.html) | `false` | Reason id, always fill by the server |
| 58 | [Text](https://www.onixs.biz/fix-dictionary/4.2/tagNum_58.html) | `false` | Explanation of the rejection, always fill by the server |