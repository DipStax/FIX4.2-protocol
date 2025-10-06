# [Market Data Request](https://www.onixs.biz/fix-dictionary/4.2/msgType_V_86.html)

| Tag | Field Name | Required | Comments |
|---|---|---|---|
| 262 | [MD Required ID](https://www.onixs.biz/fix-dictionary/4.2/tagNum_262.html) | `true` | Identifier of the request |
| 263 | [Subscription Request Type](https://www.onixs.biz/fix-dictionary/4.2/tagNum_263.html) | `true` |  |
| 264 | [Market Depth](https://www.onixs.biz/fix-dictionary/4.2/tagNum_264.html) | `true` |  |
| 265 | [MD Update Type](https://www.onixs.biz/fix-dictionary/4.2/tagNum_265.html) | `false` |  |
| 267 | [No MD Entry Type](https://www.onixs.biz/fix-dictionary/4.2/tagNum_267.html) | `true` |  |
| => 269 | [MD Entry type](https://www.onixs.biz/fix-dictionary/4.2/tagNum_269.html) | `true` | Type of data requested |
| 146 | [No Related Symbol](https://www.onixs.biz/fix-dictionary/4.2/tagNum_146.html) | `true` |  |
| => 55 | [Symbol](https://www.onixs.biz/fix-dictionary/4.2/tagNum_55.html) | `true` |  |