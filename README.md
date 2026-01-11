# sd1mux - sd1 Multiplexer
This library calls `sd1` on a socket so you don't have to. This was inspired by the new restrictions
in KDB-X where there is a limit on `sd1` handles.

For example, [qtcp](https://github.com/gyorokpeter/qtcp) used to have one socketpair per thread, but
with the new restrictions, it is no longer possible and the threads need to share a socketpair. But
if every single library did this refactoring independently, that would still be one socketpair per
library, which would still quickly breach the limit by just having multiple such libraries in the
same process. So I decided to split this into its own library that can be shared between projects.

The `selectable_socketpair` functionality required to do `sd1` on Windows is also included.

Currently there is a limit on 256 callbacks.

## Loading
The DLL should be loaded with `2:` like a q plugin. Note that even when not using KDB-X, the
`kexport` function must be called as it performs initialization. It doesn't export any functions.

The API functions must be called from another DLL that wishes to use the multiplexer functionality.

## API
* ```int sd1mux_socketpair(SOCKET \*socks)```
    Creates a selectable socketpair. This is not strictly required to use the library, but is
    included because previously it was possible to use the socketpair in both directions, i.e. not
    just "upwards" via the `sd1` logic but also "downwards" to notify a thread, which could use
    `WSAPoll` on its side of the socketpair. With the upwards notification being taken over by
    a single shared socket by `sd1mux`, the downwards communication must use another private socket,
    which can be created using this function.
* ```int sd1mux_regCallback(pipeNotifier cb)```
    Registers a callback function. Returns the ID of the callback that then should be passed into
    the other API functions. Returns -1 if all the callback slots are taken.
* ```void sd1mux_unregCallback(int id)```
    Removes a previously registered callback based on its ID. Any in-flight notifications to this
    specific instance of the callback are **not** cancelled.
* ```void sd1mux_notify(int id)```
    Sends a notification to the specified callback. The effect is that the registered callback is
    called in the main q thread. The callback is called with the ID as a parameter, which allows the
    same handler function to be used between different notification sources.
