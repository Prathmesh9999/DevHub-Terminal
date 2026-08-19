#include<stdio.h>
#include<string.h>

#include "../networking/socket.h"

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

    char buffer[1024];
    int bytesReceived=socket_receive(clientSocket,buffer,sizeof(buffer));

    if(bytesReceived>0){
        printf("Received from client : %s\n",buffer);
    }else if(bytesReceived==0){
        printf("Client disconnected.\n");
    }else{
        printf("Failed to receive data.\n");
    }
    const char* response="Hello Client";
    socket_send(clientSocket,response,(int)strlen(response));

    closesocket(clientSocket);
    closesocket(serverSocket);

    socket_cleanup();

    printf("Server shutdown complete\n");

    return 0;
}