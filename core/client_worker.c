#include "client_worker.h"

#include "../networking/protocol.h"
#include "../networking/socket.h"

#include "../core/router.h"
#include "../core/auth.h"
#include "../core/session.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

DWORD WINAPI client_worker(LPVOID argument)
{
    ClientWorkerArgs *args = (ClientWorkerArgs *)argument;

    if (args == NULL)
    {
        printf("Worker : Invalid arguments.\n");
        return 1;
    }

    ClientConnection *client = args->client;
    int clientIndex = args->clientIndex;

    if (client == NULL)
    {
        printf("Worker : Invalid client.\n");
        free(args);
        return 1;
    }

    printf("Worker started for client slot : %d.\n", clientIndex);

    while (1)
    {
        //-------------------------------------------------
        // RECEIVE DEVHUB HEADER

        DevHubHeader header;

        int headerResult =
            protocol_receive_header(
                client->session.socket,
                &header);

        if (headerResult == 0)
        {
            printf(
                "Worker: Client disconnected normally.\n");

            client_manager_remove(clientIndex);

            closesocket(client->session.socket);

            printf(
                "Worker finished for client slot %d.\n",
                clientIndex);

            free(args);

            return 0;
        }

        if (headerResult == -1)
        {
            printf(
                "Worker: Failed to receive DevHub header.\n");

            client_manager_remove(clientIndex);

            closesocket(client->session.socket);

            free(args);

            return 1;
        }
        printf("\nWorker : DevHub packet received.\n");

        printf("Worker : Version :%u\n", header.type);

        printf("Worker : Payload length : %u\n", header.payloadLength);

        //------------------------------------------------
        // RECEIVE PAYLOAD

        unsigned char payload[1024];

        if (header.payloadLength >= sizeof(payload))
        {
            printf("Worker : Payload too large.\n");
            client_manager_remove(clientIndex);
            closesocket(client->session.socket);
            free(args);
            return 1;
        }

        int payloadReceived =
            protocol_receive_payload(
                client->session.socket,
                &header,
                payload);

        if (payloadReceived == -2)
{
    printf(
        "Worker: Client disconnected normally.\n");

    client_manager_remove(clientIndex);

    closesocket(client->session.socket);

    printf(
        "Worker finished for client slot %d.\n",
        clientIndex);

    free(args);

    return 0;
}

        if (payloadReceived == -1)
        {
            printf(
                "Worker: Failed to receive payload.\n");

            client_manager_remove(clientIndex);

            closesocket(client->session.socket);

            free(args);

            return 1;
        }
        payload[payloadReceived] = '\0';
        printf("Worker : Payload : %s\n", payload);

        //----------------------------------------------
        // ROUTER

        int routeResult = router_dispatch(&client->session, &header, payload);
        if (!routeResult)
        {
            printf("Worker : Failed to route packet.\n");
        }

        //---------------------------------------------------
        // AUTHENTICATION RESPONSE

        if (header.type == DEVHUB_MSG_AUTH)
        {
            const char *responseText;
            if (routeResult)
            {
                char username[128];

                if (auth_get_username(payload, header.payloadLength, username, sizeof(username)))
                {
                    session_authenticate(&client->session, username);
                    printf("Worker : Session authenticated : %s\n", client->session.username);
                }
                printf("Worker : Authenticated : %d\n", client->session.authenticated);
                responseText = "AUTH_SUCCESS";
            }
            else
            {
                responseText = "AUTH FAILURE";
            }
            //--------------------------------------------------
            // BUILD RESPONE HEADER

            DevHubHeader responseHeader;

            responseHeader.version = DEVHUB_PROTOCOL_VERSION;
            responseHeader.type = DEVHUB_MSG_RESPONSE;
            responseHeader.payloadLength = (uint32_t)strlen(responseText);

            //----------------------------------------------------
            // BUILD RESPONSE PACKET

            unsigned char responsePacket[DEVHUB_HEADER_SIZE + 64];

            int responseSize = protocol_build_packet(&responseHeader, (const unsigned char *)responseText, responsePacket);

            if (responseSize < 0)
                printf("Worker : Failed to build AUTH response.\n");
            else
            {
                int sent = socket_send(client->session.socket, (const char *)responsePacket, responseSize);
                if (sent == responseSize)
                {
                    printf("Worker : AUTH response sent : %s\n", responseText);
                }
                else
                {
                    printf("Worker : Failed to send AUTH response.\n");
                }
            }
        }
    }
    // ---------------------------------------------------------
    // Cleanup
    // ---------------------------------------------------------

    client_manager_remove(clientIndex);

    closesocket(client->session.socket);

    printf(
        "Worker finished for client slot %d.\n",
        clientIndex);
    free(args);
    return 0;
}
