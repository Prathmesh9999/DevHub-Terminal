#include<stdio.h>
#include<stdint.h>
#include<string.h>

#include "protocol.h"

int main(){
    DevHubHeader original;

    original.version=DEVHUB_PROTOCOL_VERSION;
    original.type=DEVHUB_MSG_CHAT;
    original.payloadLength=12;

    unsigned char buffer[DEVHUB_HEADER_SIZE];
    printf("Original header : \n");
    printf("Version :%u \n",original.version);
    printf("Type : %u\n",original.type);
    printf("Payload length : %u\n",original.payloadLength);

    protocol_encode_header(&original,buffer);

    printf("\nEncoded bytes : \n");
    for(int i=0;i<DEVHUB_HEADER_SIZE;i++){
        printf("%02X ",buffer[i]);
    }
    printf("\n");
    DevHubHeader decoded;

    protocol_decode_header(buffer,&decoded);
    printf("\nDecoded header : \n");
    printf("Version :%u \n",original.version);
    printf("Type : %u\n",original.type);
    printf("Payload length : %u\n",original.payloadLength);

    printf("\nBUILDING COMPLETE PACKET...\n");
    const char* message="hello server";
    DevHubHeader packetHeader;
    packetHeader.version=DEVHUB_PROTOCOL_VERSION;
    packetHeader.type=DEVHUB_MSG_CHAT;
    packetHeader.payloadLength=(uint32_t)strlen(message);

    unsigned char packet[DEVHUB_HEADER_SIZE+1024];

    int packetSize=protocol_build_packet(&packetHeader,(const unsigned char *)message,packet);

    printf("Packet Size : %d bytes\n",packetSize);
    printf("Packet bytes : \n");

    for(int i=0;i<packetSize;i++){
        printf("%02X ",packet[i]);
    }

    printf("\n");

    printf("\nTESTING protocol validation...\n");
    DevHubHeader valid;
    valid.version=DEVHUB_PROTOCOL_VERSION;
    valid.type=DEVHUB_MSG_CHAT;
    valid.payloadLength=12;

    printf("Valid Header : %s\n",protocol_validate_header(&valid)?"PASS":"FAIL");

    DevHubHeader invalidVersion;

invalidVersion.version = 99;
invalidVersion.type = DEVHUB_MSG_CHAT;
invalidVersion.payloadLength = 12;

printf(
    "Invalid version: %s\n",
    protocol_validate_header(&invalidVersion)
        ? "PASS"
        : "FAIL"
);
DevHubHeader invalidType;

invalidType.version = DEVHUB_PROTOCOL_VERSION;
invalidType.type = 99;
invalidType.payloadLength = 12;

printf(
    "Invalid type: %s\n",
    protocol_validate_header(&invalidType)
        ? "PASS"
        : "FAIL"
);

DevHubHeader invalidLength;

invalidLength.version = DEVHUB_PROTOCOL_VERSION;
invalidLength.type = DEVHUB_MSG_CHAT;
invalidLength.payloadLength =
    DEVHUB_MAX_PAYLOAD_SIZE + 1;

printf(
    "Invalid payload length: %s\n",
    protocol_validate_header(&invalidLength)
        ? "PASS"
        : "FAIL"
);

    return 0;
}