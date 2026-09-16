#ifndef DEVHUB_CLIENT_WORKER_H
#define DEVHUB_CLIENT_WORKER_H

#include "client_manager.h"
#include<windows.h>

typedef struct {
    ClientConnection * client;
    int clientIndex;
}ClientWorkerArgs;

DWORD WINAPI client_worker(LPVOID argument);
#endif