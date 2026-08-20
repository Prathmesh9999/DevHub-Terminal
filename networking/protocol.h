#ifndef DEVHUB_PROTOCOL_H
#define DEVHUB_PROTOCOL_H

#include <stdint.h>
#include<winsock2.h>

#define DEVHUB_PROTOCOL_VERSION 1
#define DEVHUB_HEADER_SIZE 6
#define DEVHUB_MAX_PAYLOAD_SIZE 1080

typedef enum
{
    DEVHUB_MSG_AUTH = 0x01,
    DEVHUB_MSG_CHAT = 0x02,
    DEVHUB_MSG_FILE_UPLOAD = 0x03,
    DEVHUB_MSG_FILE_DOWNLOAD = 0x04,
    DEVHUB_MSG_COMMAND = 0x05,
    DEVHUB_MSG_RESPONSE = 0x06,
} MessageType;

typedef struct
{
    uint8_t version;
    uint8_t type;
    uint32_t payloadLength;
} DevHubHeader;

int protocol_encode_header(const DevHubHeader *header,unsigned char *buffer);

int protocol_decode_header(const unsigned char *buffer,DevHubHeader *header);

int protocol_build_packet(const DevHubHeader *header, const unsigned char *payload, unsigned char *buffer);

int protocol_receive_header(SOCKET sock,DevHubHeader *header);

int protocol_receive_payload(SOCKET sock,const DevHubHeader *header,unsigned char *buffer);

int protocol_validate_header(const DevHubHeader* header);
#endif