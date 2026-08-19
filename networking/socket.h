#ifndef DEVHUB_SOCKET_H
#define DEVHUB_SOCKET_H

#include <winsock2.h>

int socket_initialize(void);
void socket_cleanup(void);
SOCKET socket_create_tcp(void);

int socket_bind(SOCKET sock, const char *ip, unsigned short port);
int socket_listen(SOCKET sock, int backlog);
SOCKET socket_accept(SOCKET serverSock);

int socket_connect(SOCKET sock,const char * ip,unsigned short port);

int socket_send(SOCKET sock,const char* data,int length);
int socket_receive(SOCKET sock,char *buffer,int bufferSize);
#endif