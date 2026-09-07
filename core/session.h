#ifndef DEVHUB_SESSION_H
#define DEVHUB_SESSION_H

#include "../networking/socket.h"

typedef struct
{
    SOCKET socket;
    int authenticated;
    char username[128];

} ClientSession;

void session_init(ClientSession* session,SOCKET socket);
int session_authenticate(ClientSession* session,const char* username);
int session_is_authenticated(const ClientSession* session);
void session_logout(ClientSession * session);

#endif