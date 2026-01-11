#define KXVER 3
#include "include/sd1mux.h"
#include <synchapi.h>
#include "k.h"

int dumb_socketpair(SOCKET socks[2], int make_overlapped);

static SOCKET hReadPipe, hWritePipe;
static pipeNotifier callbacks[256];
static int callbackSq = 0;

static CRITICAL_SECTION cs;

static K processPipe(int handle) {
    unsigned char buffer[1];
    recv(handle, (char*)buffer, 1, 0);
    int id = buffer[0];
    if (callbacks[id] != NULL) {
        callbacks[id](id);
    }
    return (K)0;
}

int sd1mux_regCallback(pipeNotifier cb) {
    EnterCriticalSection(&cs);
    int origSq = callbackSq;
    callbackSq+=1;
    while(callbacks[callbackSq] != NULL) {
        callbackSq+=1;
        if (callbackSq>=256) callbackSq = 0;
        if (callbackSq == origSq) {
            LeaveCriticalSection(&cs);
            return -1;
        }
    }
    callbacks[callbackSq] = cb;
    LeaveCriticalSection(&cs);
    return callbackSq;
}

void sd1mux_unregCallback(int id) {
    EnterCriticalSection(&cs);
    callbacks[id] = NULL;
    LeaveCriticalSection(&cs);
}

void sd1mux_notify(int id) {
    char buffer[1] = {id};
    while (0 == send(hWritePipe, buffer, 1, 0));
}

int sd1mux_socketpair(SOCKET *socks) {
    return dumb_socketpair(socks, 1);
}

K kexport(K x) {
    SOCKET socks[2];
    if (dumb_socketpair(socks, 1) != 0) {
        return orr("dumb_socketpair");
    }
    hReadPipe = socks[0];
    hWritePipe = socks[1];
    K result = sd1(-(int)hReadPipe, processPipe);
    if (result == NULL) {
        return krr("failed to install pipe notify handler");
    }
    r0(result);
    InitializeCriticalSection(&cs);

    return xD(ktn(KS,0), ktn(0,0));
}
