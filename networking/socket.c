#include "socket.h"
#include <ws2tcpip.h>

int socket_initialize()
{

    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0)return 0;
    return 1;
}

void socket_cleanup()
{
    WSACleanup();
}

SOCKET socket_create_tcp()
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sock == INVALID_SOCKET)return INVALID_SOCKET;

    return sock;
}

int socket_bind(SOCKET sock, const char *ip, unsigned short port)
{
    struct sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    int result = inet_pton(AF_INET, ip, &serverAddress.sin_addr);
    if (result != 1)return 0;

    result = bind(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

    if (result == SOCKET_ERROR)return 0;

    return 1;
}

int socket_listen(SOCKET sock,int backlog){
    if(listen(sock,backlog)==SOCKET_ERROR)return 0;

    return 1;
}

SOCKET socket_accept(SOCKET serverSock){
    SOCKET clientSock=accept(serverSock,NULL,NULL);

    return clientSock;
}

int socket_connect(SOCKET sock,const char * ip,unsigned short port){
    struct sockaddr_in serverAddress;
    serverAddress.sin_family=AF_INET;
    serverAddress.sin_port=htons(port);

    int result=inet_pton(AF_INET,ip,&serverAddress.sin_addr);

    if(result!=1)return 0;
    result=connect(sock,(struct sockaddr*)&serverAddress,sizeof(serverAddress));

    if(result==SOCKET_ERROR)return 0;

    return 1;
}

int socket_send(SOCKET sock,const char* data,int length){
    int result=send(sock,data,length,0);
    if(result==SOCKET_ERROR){
        return 0;
    }
    return result;
}

int socket_receive(SOCKET sock,char* buffer,int bufferSize){
    int result=recv(sock,buffer,bufferSize-1,0);
    if(result==SOCKET_ERROR)return -1;

    if(result==0)return 0;

    buffer[result]='\0';

    return result;
}
int socket_receive_all(SOCKET sock,char* buffer,int length){
    int totalReceived=0;
    while(totalReceived<length){
        int result=recv(sock,buffer+totalReceived,length-totalReceived,0);
        if(result==SOCKET_ERROR)return -1;
        if(result==0)return 0;

        totalReceived+=result;
    }
    return totalReceived;
}