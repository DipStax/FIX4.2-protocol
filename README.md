# Epitech-Principe-financier-de-base-et-architecture

## CLI Client:

### Command:

#### User:

```
logon -u UserId
```
Send a request to logon with the **UserId** passed in argument.

```
logout
```
Send a request to logout of the actual connected account, nothing is done if the user isn't connected.

```
status
```
Display the context's information (all the status).

```
fresh -m Market -s Side -d Depth -i Id
```
Request a refresh on **Market** chosing the **Side** (both/ask/bid), the **Depth** (full/top/N) and providing an **Id**

```
sub -m Market -s Side -d Depth -i Id
```
Request a refresh on **Market** chosing the **Side** (both/ask/bid), the **Depth** (full/top/N) and providing an **Id**

```
unsub -m Market -s Side -d Depth -i Id
```
Request a refresh on **Market** chosing the **Side** (both/ask/bid), the **Depth** (full/top/N) and the target **Id**
