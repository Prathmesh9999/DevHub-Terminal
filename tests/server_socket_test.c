#include<stdio.h>
#include<string.h>

#include "../networking/socket.h"
#include"../networking/protocol.h"

int main(){
    printf("STARTING DevHub TCP server...\n");
    if(!socket_initialize()){
        printf("FAILED to initialize WINSOCK\n");
        return 1;
    }
    SOCKET serverSocket=socket_create_tcp();
    if(serverSocket==INVALID_SOCKET){
        printf("FAILED to create server Socket\n");
        return 1;
    }
    printf("TCP socket created\n");
    if(!socket_bind(serverSocket,"127.0.0.1",5000)){
        printf("FAILED to bind socket\n");
        closesocket(serverSocket);
        socket_cleanup();
        return 1;
    }
    printf("SOCKET bound to 127.0.0.1:5000\n");
     if (!socket_listen(serverSocket, 10))
    {
        printf("Failed to listen.\n");

        closesocket(serverSocket);
        socket_cleanup();

        return 1;
    }

    printf("Server is listening...\n");
    printf("Waiting for a client...\n");

    SOCKET clientSocket=socket_accept(serverSocket);
    if(clientSocket==INVALID_SOCKET){
        printf("Failed to accept client\n");
        closesocket(serverSocket);
        socket_cleanup();
        return 1;
    }

    printf("Client connected\n");

    DevHubHeader header;

if (protocol_receive_header(
        clientSocket,
        &header) != 1)
{
    printf("Failed to receive DevHub header.\n");

    closesocket(clientSocket);
    closesocket(serverSocket);
    socket_cleanup();

    return 1;
}

printf("DevHub packet received.\n");

printf("Version: %u\n", header.version);
printf("Type: %u\n", header.type);
printf(
    "Payload length: %u\n",
    header.payloadLength
);

unsigned char payload[1024];
if (header.payloadLength >= sizeof(payload))
{
    printf("Payload too large.\n");

    closesocket(clientSocket);
    closesocket(serverSocket);
    socket_cleanup();

    return 1;
}
int payloadReceived = protocol_receive_payload(
    clientSocket,
    &header,
    payload
);

if (payloadReceived < 0)
{
    printf("Failed to receive payload.\n");

    closesocket(clientSocket);
    closesocket(serverSocket);
    socket_cleanup();

    return 1;
}
payload[payloadReceived] = '\0';

printf("Payload: %s\n",payload);

const char* responseMessage="Hello Client";
DevHubHeader responseHeader;

responseHeader.version=DEVHUB_PROTOCOL_VERSION;
responseHeader.type=DEVHUB_MSG_RESPONSE;
responseHeader.payloadLength=(uint32_t)strlen(responseMessage);

unsigned char responsePacket[DEVHUB_HEADER_SIZE+1024];

int responsePacketSize=protocol_build_packet(&responseHeader,(const unsigned char*)responseMessage,responsePacket);
if (responsePacketSize < 0)
{
    printf("Failed to build response packet.\n");
 
    closesocket(clientSocket);
    closesocket(serverSocket);
    socket_cleanup();

    return 1;
}

int responseBytesSent=socket_send(clientSocket,(const char *)responsePacket,responsePacketSize);

if(responseBytesSent!=responsePacketSize){
    printf("FAILED TO SEND RESPONCE PACKET\n");
    closesocket(clientSocket);
    closesocket(serverSocket);
    socket_cleanup();
    return 1;
}
printf("DevHub response packet sent.\n");
    // char buffer[1024];
    // int bytesReceived=socket_receive(clientSocket,buffer,sizeof(buffer));

    // if(bytesReceived>0){
    //     printf("Received from client : %s\n",buffer);
    // }else if(bytesReceived==0){
    //     printf("Client disconnected.\n");
    // }else{
    //     printf("Failed to receive data.\n");
    // }
    // const char* response="Hello Client";
    // socket_send(clientSocket,response,(int)strlen(response));

    closesocket(clientSocket);
    closesocket(serverSocket);

    socket_cleanup();

    printf("Server shutdown complete\n");

    return 0;
}