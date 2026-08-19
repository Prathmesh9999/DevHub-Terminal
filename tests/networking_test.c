#include <stdio.h>

#include "../networking/socket.h"

int main()
{
    printf("Initializing Winsock...\n");

    if (!socket_initialize())
    {
        printf("FAILED TO INITIALIZED WINSOCK\n");
        return 1;
    }
    printf("Winsock initialized SUCCESSFULLY\n");
    SOCKET sock = socket_create_tcp();
    if (sock == INVALID_SOCKET)
    {
        printf("FAILED TO CREATE TCP SOCKET\n");
        socket_cleanup();
        return 1;
    }
    printf("TCP Socket created successfully\n");
    closesocket(sock);
    socket_cleanup();
    printf("Networking cleanup complete.\n");
    return 0;
}