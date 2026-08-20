#include"chat.h"
#include<stdio.h>

int chat_handle(const unsigned char* payload,unsigned int payloadLength){
    if(payload==NULL){
        return 0;
    }

    printf("Chat Handler : ");
    for(unsigned int i=0;i<payloadLength;i++){
        putchar(payload[i]);
    }

    putchar('\n');
    return 1;
}