#include<stdio.h>

#include "../core/auth.h"

int main(){
    unsigned char buffer[256];
    printf("\nTesting AUTH payload builder...\n");

    int payloadSize=auth_build_payload("praxx","devhub999",buffer,sizeof(buffer));

    if(payloadSize<0){
        printf("AUTH payload build FAILED\n");
        return 1;
    }
    printf("AUTH payload size : %d bytes\n",payloadSize);

    printf("Payload Bytes : ");
    for(int i=0;i<payloadSize;i++){
        printf("%02X ",buffer[i]);
    }

    printf("\n");
    return 0;
}