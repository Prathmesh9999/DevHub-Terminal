#include <stdio.h>
#include<string.h>

#include "../networking/socket.h"
#include "../networking/protocol.h"

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

    DevHubHeader header;

    header.version=DEVHUB_PROTOCOL_VERSION;
    header.type=DEVHUB_MSG_CHAT;
    header.payloadLength=(uint32_t)strlen("Hello Server");

    const char *message = "Hello Server";
    
    unsigned char packet[DEVHUB_HEADER_SIZE+1024];

    int packetSize=protocol_build_packet(&header,(const unsigned char*)message,packet);
    
    if(packetSize<0){
        printf("FAILED TO BUILD PACKET\n");
        closesocket(clientSocket);
        socket_cleanup();
        return 1;
    }

    int byteSent=socket_send(clientSocket,(const char*)packet,packetSize);
    if(byteSent!=packetSize){
        printf("FAILEDTO SEND COMPLETE PACKET\n");
        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    printf("DevHub packet sent.\n");
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

    closesocket(clientSocket);

    socket_cleanup();

    printf("Client shutdown complete.\n");

    return 0;
}