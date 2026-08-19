#include <stdio.h>

#include "../networking/socket.h"

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

    if (!socket_connect(clientSocket,"127.0.0.1",5000))
    {
        printf("Failed to connect to server.\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    printf("Connected to DevHub server!\n");

    closesocket(clientSocket);

    socket_cleanup();

    printf("Client shutdown complete.\n");

    return 0;
}