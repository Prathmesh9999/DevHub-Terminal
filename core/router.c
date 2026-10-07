#include"router.h"
#include "chat.h"
#include "auth.h"
#include "file_handler.h"
#include "file_download.h"
#include "session.h"
#include<stdio.h>


int router_dispatch(ClientSession *session,const DevHubHeader * header,const unsigned char* payload){
    if(session==NULL||header==NULL){return 0;
    }

    if(header->type!=DEVHUB_MSG_AUTH&&!session_is_authenticated(session)){
        printf("Router : Client is not authenticated.\n");
        return 0;
    }
    switch (header->type)
    {
    case DEVHUB_MSG_AUTH:
        printf("Router : AUTH message\n");
        return auth_handle(payload,header->payloadLength);
    case DEVHUB_MSG_CHAT:
        printf("Router : CHAT message\n");
        return chat_handle(payload,header->payloadLength);
    case DEVHUB_MSG_FILE_UPLOAD:
        printf("Router : FILE_UPLOAD message\n");
        if(!session_is_authenticated(session)){
            printf("Router : Client not authenticated\n");
            return 0;
        }
        return file_upload_handle(payload,header->payloadLength);
    case DEVHUB_MSG_FILE_DOWNLOAD:
        printf("Router : FILE_DOWNLOAD message\n");
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

int router_handle_file_download(
    ClientSession *session,
    const DevHubHeader *header,
    const unsigned char *payload,
    unsigned char *responseBuffer,
    unsigned int responseBufferSize)
{
    if (session == NULL)
    {
        printf(
            "Router : Invalid session.\n"
        );

        return -1;
    }

    if (header == NULL)
    {
        printf(
            "Router : Invalid header.\n"
        );

        return -1;
    }

    if (payload == NULL)
    {
        printf(
            "Router : Invalid payload.\n"
        );

        return -1;
    }

    if (responseBuffer == NULL)
    {
        printf(
            "Router : Invalid response buffer.\n"
        );

        return -1;
    }

    if (!session_is_authenticated(session))
    {
        printf(
            "Router : Client not authenticated.\n"
        );

        return -1;
    }

    if (header->type != DEVHUB_MSG_FILE_DOWNLOAD)
    {
        printf(
            "Router : Invalid message type for download.\n"
        );

        return -1;
    }

    printf(
        "Router : FILE_DOWNLOAD message\n"
    );

    return file_download_handle(
        payload,
        header->payloadLength,
        responseBuffer,
        responseBufferSize
    );
}