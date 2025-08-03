# Server



## FIX specification

You can find the specification of the supported message and their feild in the [`spec`](./spec/README.md) folder.
This provide a table as:

| Tag | Field Name | Required | Comments |
|---|---|---|---|
| 1 | Example Field | `true` | This is an example field |

- `Tag`: the tag reference by the field.
- `Field Name`: the name of the field, the link is a reference to the FIX4.2 protocol manual followed by the server (as the title).
- `Required`: specify if it is required (most of the time it is, because this **PoC**)
- `Comments`: additionnal comment on the field and what it representes.