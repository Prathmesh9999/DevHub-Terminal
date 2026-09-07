#ifndef DEVHUB_ROUTER_H
#define DEVHUB_ROUTER_H

#include"../networking/protocol.h"
#include"session.h"

int router_dispatch(ClientSession *session,const DevHubHeader * header,const unsigned char* payload);

#endif