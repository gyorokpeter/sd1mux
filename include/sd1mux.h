#ifndef SD1MUX_H_
#define SD1MUX_H_

#include <winsock2.h>

#ifdef __cplusplus
extern"C"{
#endif

typedef void (*pipeNotifier)(int id);
int sd1mux_regCallback(pipeNotifier cb);
void sd1mux_unregCallback(int id);
void sd1mux_notify(int id);
int sd1mux_socketpair(SOCKET *socks);

#ifdef __cplusplus
}
#endif

#endif