#include"router.h"
#include "chat.h"
#include<stdio.h>

int router_dispatch(const DevHubHeader * header,const unsigned char* payload){
    if(header==NULL){return 0;
    }

    switch (header->type)
    {
    case DEVHUB_MSG_AUTH:
        printf("Router : AUTH message\n");
        break;
    case DEVHUB_MSG_CHAT:
        printf("Router : CHAT message\n");
        return chat_handle(payload,header->payloadLength);
    case DEVHUB_MSG_FILE_DOWNLOAD:
        printf("Router : FILE_DOWNLOAD message\n");
        break;
    case DEVHUB_MSG_FILE_UPLOAD:
        printf("Router : FILE_UPLOAD message\n");
        break;
    case DEVHUB_MSG_COMMAND:
        printf("Router : COMMAND message\n");
        break;
    case DEVHUB_MSG_RESPONSE:
        printf("Router : RESPONSE message\n");
        break;
    
    default:
    printf("Router : Unknown message types\n");
        return 0;
    }

    return 1;
}