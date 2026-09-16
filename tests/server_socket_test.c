#include <stdio.h>
#include <stdlib.h>


#include "../networking/socket.h"

#include "../core/client_manager.h"
#include "../core/client_worker.h"
#include <windows.h>

int main()
{
    printf("STARTING DevHub TCP server...\n");

    // ---------------------------------------------------------
    // 1. Initialize Winsock
    // ---------------------------------------------------------

    if (!socket_initialize())
    {
        printf("FAILED TO INITIALIZE WINSOCK\n");
        return 1;
    }

    // ---------------------------------------------------------
    // 2. Initialize Client Manager
    // ---------------------------------------------------------

    client_manager_init();

    printf("Client manager initialized.\n");

    // ---------------------------------------------------------
    // 3. Create TCP server socket
    // ---------------------------------------------------------

    SOCKET serverSocket =
        socket_create_tcp();

    if (serverSocket == INVALID_SOCKET)
    {
        printf("FAILED TO CREATE SERVER SOCKET\n");

        socket_cleanup();

        return 1;
    }

    printf("TCP socket created.\n");

    // ---------------------------------------------------------
    // 4. Bind
    // ---------------------------------------------------------

    if (!socket_bind(
            serverSocket,
            "127.0.0.1",
            5000))
    {
        printf("FAILED TO BIND SOCKET\n");

        closesocket(serverSocket);
        socket_cleanup();

        return 1;
    }

    printf(
        "SOCKET BOUND TO 127.0.0.1:5000\n");

    // ---------------------------------------------------------
    // 5. Listen
    // ---------------------------------------------------------

    if (!socket_listen(
            serverSocket,
            10))
    {
        printf("FAILED TO LISTEN\n");

        closesocket(serverSocket);
        socket_cleanup();

        return 1;
    }

    printf("SERVER IS LISTENING...\n");
    printf("Waiting for clients...\n");

    // =========================================================
    // 6. ACCEPT CLIENTS FOREVER
    // =========================================================

    while (1)
    {
        printf(
            "\nWaiting for a new client...\n");

        // -----------------------------------------------------
        // Accept a new client
        // -----------------------------------------------------

        SOCKET clientSocket =
            socket_accept(serverSocket);

        if (clientSocket == INVALID_SOCKET)
        {
            printf(
                "FAILED TO ACCEPT CLIENT\n");

            continue;
        }

        printf(
            "CLIENT CONNECTED\n");

        // -----------------------------------------------------
        // Add client to manager
        // -----------------------------------------------------

        int clientIndex =
            client_manager_add(clientSocket);

        if (clientIndex == -1)
        {
            printf(
                "MAXIMUM CLIENT LIMIT REACHED\n");

            closesocket(clientSocket);

            continue;
        }

        printf(
            "Client assigned to slot: %d\n",
            clientIndex);

        // -----------------------------------------------------
        // Get client from manager
        // -----------------------------------------------------

        ClientConnection *client =
            client_manager_get(clientIndex);

        if (client == NULL)
        {
            printf(
                "FAILED TO GET CLIENT CONNECTION\n");

            client_manager_remove(clientIndex);

            closesocket(clientSocket);

            continue;
        }

        // -----------------------------------------------------
        // Create worker arguments
        // -----------------------------------------------------

        ClientWorkerArgs *args =
            malloc(sizeof(ClientWorkerArgs));

        if (args == NULL)
        {
            printf(
                "FAILED TO ALLOCATE WORKER ARGUMENTS\n");

            client_manager_remove(clientIndex);

            closesocket(clientSocket);

            continue;
        }

        args->client = client;
        args->clientIndex = clientIndex;

        // -----------------------------------------------------
        // Create worker thread
        // -----------------------------------------------------

        HANDLE threadHandle = CreateThread(NULL, 0, client_worker, args, 0, NULL);

        if (threadHandle == NULL)
        {
            printf(
                "FAILED TO CREATE WORKER THREAD\n");

            free(args);

            client_manager_remove(clientIndex);

            closesocket(clientSocket);

            continue;
        }

        printf(
            "Worker thread created for client slot %d.\n",
            clientIndex);

        // -----------------------------------------------------
        // Close our HANDLE to the thread
        // -----------------------------------------------------

        CloseHandle(threadHandle);

        printf(
            "Server is ready to accept another client.\n");
    }

    // ---------------------------------------------------------
    // Server cleanup
    // ---------------------------------------------------------

    closesocket(serverSocket);

    socket_cleanup();

    return 0;
}