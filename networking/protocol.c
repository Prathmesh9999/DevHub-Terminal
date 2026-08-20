#include"protocol.h"
#include<winsock2.h>
#include<string.h>
#include"socket.h"

int protocol_encode_header(const DevHubHeader * header,unsigned char* buffer){
    buffer[0]=header->version;
    buffer[1]=header->type;

    uint32_t networkLength=htonl(header->payloadLength);

    memcpy(buffer+2,&networkLength,sizeof(networkLength));

    return DEVHUB_HEADER_SIZE;

}

int protocol_decode_header(const unsigned char* buffer,DevHubHeader* header){
    header->version=buffer[0];
    header->type=buffer[1];
    uint32_t networkLength;

    memcpy(&networkLength,buffer+2,sizeof(networkLength));

    header->payloadLength=ntohl(networkLength);

    return DEVHUB_HEADER_SIZE;
}

int protocol_build_packet(const DevHubHeader *header, const unsigned char *payload, unsigned char *buffer){
    int headerSize=protocol_encode_header(header,buffer);

    if(headerSize!=DEVHUB_HEADER_SIZE)return -1;

    memcpy(buffer+DEVHUB_HEADER_SIZE,payload,header->payloadLength);

    return DEVHUB_HEADER_SIZE+header->payloadLength;
}

int protocol_receive_header(SOCKET sock,DevHubHeader *header)
{
    unsigned char buffer[DEVHUB_HEADER_SIZE];

    int result = socket_receive_all(sock,(char *)buffer,DEVHUB_HEADER_SIZE);

    if (result != DEVHUB_HEADER_SIZE)
    {
        return -1;
    }

    result = protocol_decode_header( buffer,header);

    if (result != DEVHUB_HEADER_SIZE)
    {
        return -1;
    }

    if(!protocol_validate_header(header)){
        return -1;
    }

    return 1;
}

int protocol_receive_payload(SOCKET sock,const DevHubHeader *header,unsigned char *buffer)
{
    if (header->payloadLength == 0)
    {
        return 0;
    }

    return socket_receive_all(
        sock,
        (char *)buffer,
        (int)header->payloadLength
    );
}

int protocol_validate_header(const DevHubHeader* header){

    if(header==NULL)return 0;

    if(header->version!=DEVHUB_PROTOCOL_VERSION)return 0;

    if(header->type<DEVHUB_MSG_AUTH||header->type>DEVHUB_MSG_RESPONSE)return 0;

    if(header->payloadLength>DEVHUB_MAX_PAYLOAD_SIZE)return 0;

    return 1;
}