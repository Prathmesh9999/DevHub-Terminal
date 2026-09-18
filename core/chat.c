#include"chat.h"
#include "../networking/protocol.h"
#include<stdio.h>
#include<string.h>

int chat_handle(const unsigned char* payload,unsigned int payloadLength){
    if(payload==NULL){
        printf("CHat Handlerr : Invalid Payload\n");
        return 0;
    }

    if(payloadLength==0){
        printf("Chat Handler : Empty messag\n");
        return 0;
    }

    if(payloadLength>DEVHUB_MAX_PAYLOAD_SIZE){
        printf("Chat Handler : Messaeg too larger\n");
        return 0;
    }

    printf("Chat Handler : Message = ");
    for(unsigned int i=0;i<payloadLength;i++){
        putchar(payload[i]);
    }

    putchar('\n');
    return 1;
}


int chat_build_response(unsigned char* buffer,unsigned int bufferSize){
    const char * response="CHAT_OK";
    unsigned int responseLength=(unsigned int)strlen(response);

    if(buffer==NULL)return -1;
    if(bufferSize<responseLength)return -1;

    memcpy(buffer,response,responseLength);

    return (int)responseLength;
}
