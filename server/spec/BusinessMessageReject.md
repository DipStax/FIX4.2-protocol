# [Business Message Reject]()

| Tag | Field Name | Required | Comments |
|---|---|---|---|
| 45 | [Reference Sequence Number](https://www.onixs.biz/fix-dictionary/4.2/tagNum_45.html) | `false` | Reference to the `Message Sequence Number` of the rejected message, always fill by the server |
| 372 | [Reference Message Type](https://www.onixs.biz/fix-dictionary/4.2/tagNum_372.html) | `true` | `Message Type` of the rejected message, always fill by the server |
| 379 | [Business Reject Reference Id](https://www.onixs.biz/fix-dictionary/4.2/tagNum_379.html) | `false` | Business level ID referenced by the rejected message, always fill by the server |
| 373 | [Business Reject Reason](https://www.onixs.biz/fix-dictionary/4.2/tagNum_380.html) | `true` | Reason id |
| 58 | [Text](https://www.onixs.biz/fix-dictionary/4.2/tagNum_58.html) | `false` | Explanation of the rejection, always fill by the server |