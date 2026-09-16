#ifndef DEVHUB_CLIENT_MANAGER_H
#define DEVHUB_CLIENT_MANAGER_H

#include "session.h"

#define DEVHUB_MAX_CLIENTS 10

typedef struct
{
    ClientSession session;
    int active;

} ClientConnection;

void client_manager_init();
int client_manager_add(SOCKET socket);
void client_manager_remove(int index);
ClientConnection* client_manager_get(int index);

#endif