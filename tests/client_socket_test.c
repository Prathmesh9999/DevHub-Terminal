#include <stdio.h>
#include<stdint.h>
#include <string.h>

#include "../networking/socket.h"
#include "../networking/protocol.h"
#include "../core/auth.h"

int main()
{
    printf("Starting DevHub TCP client...\n");

    // ---------------------------------------------------------
    // 1. Initialize Winsock
    // ---------------------------------------------------------

    if (!socket_initialize())
    {
        printf("FAILED TO INITIALIZE WINSOCK\n");
        return 1;
    }

    // ---------------------------------------------------------
    // 2. Create TCP socket
    // ---------------------------------------------------------

    SOCKET clientSocket = socket_create_tcp();

    if (clientSocket == INVALID_SOCKET)
    {
        printf("FAILED TO CREATE TCP SOCKET\n");

        socket_cleanup();
        return 1;
    }

    printf("TCP socket created.\n");

    // ---------------------------------------------------------
    // 3. Connect to DevHub server
    // ---------------------------------------------------------

    printf("Connecting to 127.0.0.1:5000...\n");

    if (!socket_connect(
            clientSocket,
            "127.0.0.1",
            5000))
    {
        printf("FAILED TO CONNECT TO DEVHUB SERVER\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    printf("Connected to DevHub server!\n");

    // =========================================================
    // AUTHENTICATION
    // =========================================================

    const char *username = "praxx";
    const char *password = "devhub999";

    // ---------------------------------------------------------
    // 4. Build authentication payload
    // ---------------------------------------------------------

    unsigned char authPayload[256];

    int authPayloadSize =
        auth_build_payload(
            username,
            password,
            authPayload,
            sizeof(authPayload));

    if (authPayloadSize < 0)
    {
        printf("FAILED TO BUILD AUTH PAYLOAD\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    printf(
        "Auth payload built. Size: %d bytes\n",
        authPayloadSize);

    // ---------------------------------------------------------
    // 5. Create DevHub AUTH header
    // ---------------------------------------------------------

    DevHubHeader authHeader;

    authHeader.version =
        DEVHUB_PROTOCOL_VERSION;

    authHeader.type =
        DEVHUB_MSG_AUTH;

    authHeader.payloadLength =
        (uint32_t)authPayloadSize;

    // ---------------------------------------------------------
    // 6. Build complete DevHub packet
    // ---------------------------------------------------------

    unsigned char authPacket[DEVHUB_HEADER_SIZE + 256];

    int authPacketSize =
        protocol_build_packet(
            &authHeader,
            authPayload,
            authPacket);

    if (authPacketSize < 0)
    {
        printf("FAILED TO BUILD AUTH PACKET\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    printf(
        "Auth packet built. Size: %d bytes\n",
        authPacketSize);

    // ---------------------------------------------------------
    // 7. Send AUTH packet
    // ---------------------------------------------------------

    int bytesSent =
        socket_send(
            clientSocket,
            (const char *)authPacket,
            authPacketSize);

    if (bytesSent != authPacketSize)
    {
        printf("FAILED TO SEND AUTH PACKET\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    printf("AUTH packet sent.\n");

    // =========================================================
    // RECEIVE SERVER RESPONSE
    // =========================================================

    // ---------------------------------------------------------
    // 8. Receive response header
    // ---------------------------------------------------------

    DevHubHeader responseHeader;

    if (protocol_receive_header(
            clientSocket,
            &responseHeader) != 1)
    {
        printf(
            "FAILED TO RECEIVE SERVER RESPONSE HEADER\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    printf("\nServer response received.\n");

    printf(
        "Response version: %u\n",
        responseHeader.version);

    printf(
        "Response type: %u\n",
        responseHeader.type);

    printf(
        "Response payload length: %u\n",
        responseHeader.payloadLength);

    // ---------------------------------------------------------
    // 9. Check response payload size
    // ---------------------------------------------------------

    unsigned char responsePayload[1024];

    if (responseHeader.payloadLength >=
        sizeof(responsePayload))
    {
        printf("SERVER RESPONSE PAYLOAD TOO LARGE\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    // ---------------------------------------------------------
    // 10. Receive response payload
    // ---------------------------------------------------------

    int responsePayloadSize =
        protocol_receive_payload(
            clientSocket,
            &responseHeader,
            responsePayload);

    if (responsePayloadSize < 0)
    {
        printf(
            "FAILED TO RECEIVE SERVER RESPONSE PAYLOAD\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    responsePayload[responsePayloadSize] = '\0';

    // ---------------------------------------------------------
    // 11. Display server response
    // ---------------------------------------------------------

    printf(
        "Server response: %s\n",
        responsePayload);

    // ---------------------------------------------------------
    // 12. Close client socket
    // ---------------------------------------------------------
    printf("\nClient authenticated successfully.\n");

    /* =========================================================
       CHAT TEST
       ========================================================= */

    printf("\nSending CHAT message...\n");

    const char *chatMessage =
        "Hello from DevHub Client";

    DevHubHeader chatHeader;

    chatHeader.version =
        DEVHUB_PROTOCOL_VERSION;

    chatHeader.type =
        DEVHUB_MSG_CHAT;

    chatHeader.payloadLength =
        (uint32_t)strlen(chatMessage);

    unsigned char chatPacket[DEVHUB_HEADER_SIZE + DEVHUB_MAX_PAYLOAD_SIZE];

    int chatPacketSize =
        protocol_build_packet(
            &chatHeader,
            (const unsigned char *)chatMessage,
            chatPacket);

    if (chatPacketSize < 0)
    {
        printf("FAILED TO BUILD CHAT PACKET\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    printf(
        "CHAT packet built. Size: %d bytes\n",
        chatPacketSize);

    int chatByteSent = socket_send(clientSocket, (const char *)chatPacket, chatPacketSize);

    if (chatByteSent != chatPacketSize)
    {
        printf("FAILED TO SEND CHAT PACKET\n");
        closesocket(clientSocket);
        socket_cleanup();
        return 1;
    }
    printf("CHAT packet sent successfully.\n");
    printf("\nWaiting for CHAT response...\n");

DevHubHeader chatResponseHeader;

int chatResponseHeaderResult =
    protocol_receive_header(
        clientSocket,
        &chatResponseHeader
    );

if (chatResponseHeaderResult != 1)
{
    printf("FAILED TO RECEIVE CHAT RESPONSE HEADER\n");

    closesocket(clientSocket);
    socket_cleanup();

    return 1;
}

printf("CHAT response header received.\n");

printf(
    "Response version : %u\n",
    chatResponseHeader.version
);

printf(
    "Response type : %u\n",
    chatResponseHeader.type
);

printf(
    "Response payload length : %u\n",
    chatResponseHeader.payloadLength
);

unsigned char chatResponsePayload[64];

if (chatResponseHeader.payloadLength >=
    sizeof(chatResponsePayload))
{
    printf("CHAT RESPONSE PAYLOAD TOO LARGE\n");

    closesocket(clientSocket);
    socket_cleanup();

    return 1;
}

int chatResponsePayloadSize =
    protocol_receive_payload(
        clientSocket,
        &chatResponseHeader,
        chatResponsePayload
    );

if (chatResponsePayloadSize < 0)
{
    printf("FAILED TO RECEIVE CHAT RESPONSE PAYLOAD\n");

    closesocket(clientSocket);
    socket_cleanup();

    return 1;
}

chatResponsePayload[chatResponsePayloadSize] = '\0';

printf(
    "CHAT response : %s\n",
    chatResponsePayload
);

if (strcmp(
        (const char *)chatResponsePayload,
        "CHAT_OK") == 0)
{
    printf("CHAT end-to-end test PASSED.\n");
}
else
{
    printf("CHAT end-to-end test FAILED.\n");
}
    printf("Client is staying connected.\n");
    printf("Press ENTER to disconnect this client...\n");

    getchar();
    closesocket(clientSocket);
    socket_cleanup();
    printf("Client shutdown complete.\n");

    return 0;
}