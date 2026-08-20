#ifndef DEVHUB_ROUTER_H
#define DEVHUB_ROUTER_H

#include"../networking/protocol.h"

int router_dispatch(const DevHubHeader * header,const unsigned char* payload);

#endif