#include <stdio.h>
#include<string.h>

#include "../networking/socket.h"
#include "../networking/protocol.h"
#include "../core/auth.h"

int main(void)
{
    printf("Starting DevHub TCP client...\n");

    if (!socket_initialize())
    {
        printf("Failed to initialize Winsock.\n");
        return 1;
    }

    SOCKET clientSocket = socket_create_tcp();

    if (clientSocket == INVALID_SOCKET)
    {
        printf("Failed to create client socket.\n");
        socket_cleanup();
        return 1;
    }

    printf("TCP socket created.\n");

    printf("Connecting to 127.0.0.1:5000...\n");

    if (!socket_connect(clientSocket, "127.0.0.1", 5000))
    {
        printf("Failed to connect to server.\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    printf("Connected to DevHub server!\n");

    unsigned char authPayload[256];

    int authPayloadSize=auth_build_payload("praxx","wrongPass",authPayload,sizeof(authPayload));

    if(authPayloadSize<0){
        printf("Faild to build AUTH payload\n");
        closesocket(clientSocket);
        socket_cleanup();
        return 1;
    }

    DevHubHeader authHeader;
    authHeader.version=DEVHUB_PROTOCOL_VERSION;
    authHeader.type=DEVHUB_MSG_AUTH;
    authHeader.payloadLength=(uint32_t)authPayloadSize;

    unsigned char authPacket[DEVHUB_HEADER_SIZE+DEVHUB_MAX_PAYLOAD_SIZE];

    int authPacketSize=protocol_build_packet(&authHeader,authPayload,authPacket);
    if (authPacketSize < 0)
{
    printf("Failed to build AUTH packet.\n");

    closesocket(clientSocket);
    socket_cleanup();

    return 1;
}

int bytesSent = socket_send(
    clientSocket,
    (const char *)authPacket,
    authPacketSize
);
if (bytesSent != authPacketSize)
{
    printf("Failed to send complete AUTH packet.\n");

    closesocket(clientSocket);
    socket_cleanup();

    return 1;
}

printf("AUTH packet sent.\n");


    // DevHubHeader header;

    // header.version=DEVHUB_PROTOCOL_VERSION;
    // header.type=DEVHUB_MSG_CHAT;
    // header.payloadLength=(uint32_t)strlen("Hello Server");

    // const char *message = "Hello Server";
    
    // unsigned char packet[DEVHUB_HEADER_SIZE+1024];

    // int packetSize=protocol_build_packet(&header,(const unsigned char*)message,packet);
    
    // if(packetSize<0){
    //     printf("FAILED TO BUILD PACKET\n");
    //     closesocket(clientSocket);
    //     socket_cleanup();
    //     return 1;
    // }

    // int byteSent=socket_send(clientSocket,(const char*)packet,packetSize);
    // if(byteSent!=packetSize){
    //     printf("FAILEDTO SEND COMPLETE PACKET\n");
    //     closesocket(clientSocket);
    //     socket_cleanup();

    //     return 1;
    // }

    // printf("DevHub packet sent.\n");
    // socket_send(clientSocket, message, (int)strlen(message));
    // char buffer[1024];

    // int bytesReceived = socket_receive(
    //     clientSocket,
    //     buffer,
    //     sizeof(buffer));

    // if (bytesReceived > 0)
    // {
    //     printf(
    //         "Received from server: %s\n",
    //         buffer);
    // }
    // else if (bytesReceived == 0)
    // {
    //     printf("Server disconnected.\n");
    // }
    // else
    // {
    //     printf("Failed to receive data.\n");
    // }

DevHubHeader responseHeader;

int result = protocol_receive_header(
    clientSocket,
    &responseHeader
);

if (result != 1)
{
    printf("Failed to receive AUTH response header.\n");
    return 1;
}

unsigned char responsePayload[1024];

int responsePayloadSize=protocol_receive_payload(clientSocket,&responseHeader,responsePayload);
if(responsePayloadSize<0){
    printf("Failed to receive AUTH response payload\n");
    return 1;
}
responsePayload[responsePayloadSize] = '\0';

printf(
    "Server response: %s\n",
    responsePayload
);
    // DevHubHeader responseHeader;
    // if(protocol_receive_header(clientSocket,&responseHeader)!=1){
    //      printf("Failed to receive response header.\n");
    //      closesocket(clientSocket);
    //      socket_cleanup();

    //      return 1;
    // }

    // printf("Response packet received\n");
    // printf("Response version : %u\n",responseHeader.version);
    // printf("Response type : %u\n",responseHeader.type);
    // printf("Response payload length : %u\n",responseHeader.payloadLength);

    // if(responseHeader.payloadLength>=1024){
    //     printf("RESPONSE payload is too large.\n");
    //     closesocket(clientSocket);
    //     socket_cleanup();
    //     return 1;
    // }

    // unsigned char responsePayload[1024];

    // int responsePayloadReceived=protocol_receive_payload(clientSocket,&responseHeader,responsePayload);

    // if(responsePayloadReceived<0){
    //     printf("FAILED TO RECEIVE RESPONCE PAYLOAD.\n");
    //     closesocket(clientSocket);
    //     socket_cleanup();
    //     return 1;
    // }

    // responsePayload[responsePayloadReceived]='\0';
    // printf("Response Payload :%s\n",responsePayload);

    closesocket(clientSocket);

    socket_cleanup();

    printf("Client shutdown complete.\n");

    return 0;
}