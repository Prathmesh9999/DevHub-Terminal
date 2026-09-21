#include <stdio.h>
#include <stdint.h>
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
            &chatResponseHeader);

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
        chatResponseHeader.version);

    printf(
        "Response type : %u\n",
        chatResponseHeader.type);

    printf(
        "Response payload length : %u\n",
        chatResponseHeader.payloadLength);

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
            chatResponsePayload);

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
        chatResponsePayload);

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

    /* =========================================================
       FILE UPLOAD TEST
       ========================================================= */
    printf("\nStarting FILE_UPLOAD test...\n");

    const char *filePath = "sample.cpp";
    const char *fileName = "sample.cpp";

    // OPEN FILE ----------------------------------------------

    FILE *file = fopen(filePath, "rb");
    if (file == NULL)
    {
        printf("FAILED TO OPEN FILE FOR UPLOAD\n");

        closesocket(clientSocket);
        socket_cleanup();
        return 1;
    }

    // GET FILE SIZE -----------------------------------------

    fseek(file, 0, SEEK_END);

    long fileSizeLong = ftell(file);

    fseek(file, 0, SEEK_SET);

    if (fileSizeLong < 0)
    {
        printf("FAILED TO GET FILE SIZE\n");

        fclose(file);
        closesocket(clientSocket);
        socket_cleanup();
        return 1;
    }

    uint32_t fileSize = (uint32_t)fileSizeLong;
    printf("File name : %s\n", fileName);
    printf("File Size : %u bytes\n", fileSize);

    // READ FILE ----------------------------------------------

    unsigned char fileData[512];

    if (fileSize > sizeof(fileData))
    {
        printf("FILE TOO LARGE FOR CURRENT TEST\n");

        fclose(file);
        closesocket(clientSocket);
        socket_cleanup();
        return 1;
    }

    size_t bytesRead = fread(fileData, 1, fileSize, file);

    fclose(file);

    if (bytesRead != fileSize)
    {
        printf("FAILED TO READ COMPLETE FILE\n");
        closesocket(clientSocket);
        socket_cleanup();
        return 1;
    }

    /* ---------------------------------------------------------
       BUILD FILE_UPLOAD PAYLOAD
       --------------------------------------------------------- */

    uint16_t fileNameLength = (uint16_t)strlen(fileName);

    unsigned char uploadPayload[1024];
    unsigned int offset = 0;

    uploadPayload[offset++] = (unsigned char)(fileNameLength >> 8);

    uploadPayload[offset++] = (unsigned char)(fileNameLength & 0xFF);

    memcpy(uploadPayload + offset, fileName, fileNameLength);

    offset += fileNameLength;

    /* File size - 4 bytes */

    uploadPayload[offset++] =
        (unsigned char)(fileSize >> 24);

    uploadPayload[offset++] =
        (unsigned char)(fileSize >> 16);

    uploadPayload[offset++] =
        (unsigned char)(fileSize >> 8);

    uploadPayload[offset++] =
        (unsigned char)(fileSize);

    /* File data */

    memcpy(
        uploadPayload + offset,
        fileData,
        fileSize);

    offset += fileSize;

    printf(
        "FILE_UPLOAD payload size : %u bytes\n",
        offset);

    /* ---------------------------------------------------------
       BUILD DEVHUB PACKET
       --------------------------------------------------------- */

    DevHubHeader uploadHeader;
    uploadHeader.version = DEVHUB_PROTOCOL_VERSION;
    uploadHeader.type = DEVHUB_MSG_FILE_UPLOAD;
    uploadHeader.payloadLength = (uint32_t)offset;

    unsigned char uploadPacket[DEVHUB_HEADER_SIZE + 1024];

    int uploadPacketSize = protocol_build_packet(&uploadHeader, uploadPayload, uploadPacket);

    if (uploadPacketSize < 0)
    {
        printf("FAILED TO BUILD FILE_UPLOAD PACKET\n");
        closesocket(clientSocket);
        socket_cleanup();
        return 1;
    }

    printf("FILE UPLOAD packet size : %d bytes\n", uploadPacketSize);

    /* ---------------------------------------------------------
       SEND FILE_UPLOAD PACKET
       --------------------------------------------------------- */

    int uploadBytesSent = socket_send(clientSocket, (const char *)uploadPacket, uploadPacketSize);

    if (uploadBytesSent != uploadPacketSize)
    {
        printf("FAILED TO SEND FILE_UPLOAD PACKET\n");
        closesocket(clientSocket);
        socket_cleanup();
        return 1;
    }

    printf("FILE_UPLOAD packet sent successfully\n");

    printf("\nWaiting for FILE_UPLOAD response...\n");

    DevHubHeader uploadResponseHeader;

    int uploadResponseHeaderResult =
        protocol_receive_header(
            clientSocket,
            &uploadResponseHeader);

    if (uploadResponseHeaderResult != 1)
    {
        printf(
            "FAILED TO RECEIVE FILE_UPLOAD RESPONSE HEADER\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    printf(
        "FILE_UPLOAD response header received.\n");

    printf(
        "Response version : %u\n",
        uploadResponseHeader.version);

    printf(
        "Response type : %u\n",
        uploadResponseHeader.type);

    printf(
        "Response payload length : %u\n",
        uploadResponseHeader.payloadLength);

    unsigned char uploadResponsePayload[64];

    if (uploadResponseHeader.payloadLength >=
        sizeof(uploadResponsePayload))
    {
        printf(
            "FILE_UPLOAD RESPONSE PAYLOAD TOO LARGE\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    int uploadResponsePayloadSize =
        protocol_receive_payload(
            clientSocket,
            &uploadResponseHeader,
            uploadResponsePayload);

    if (uploadResponsePayloadSize < 0)
    {
        printf(
            "FAILED TO RECEIVE FILE_UPLOAD RESPONSE PAYLOAD\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    uploadResponsePayload[uploadResponsePayloadSize] = '\0';

    printf(
        "FILE_UPLOAD response : %s\n",
        uploadResponsePayload);

    if (strcmp(
            (const char *)uploadResponsePayload,
            "FILE_UPLOAD_OK") == 0)
    {
        printf(
            "FILE_UPLOAD end-to-end test PASSED.\n");
    }
    else
    {
        printf(
            "FILE_UPLOAD end-to-end test FAILED.\n");
    }


    printf("Client is staying connected.\n");
    printf("Press ENTER to disconnect this client...\n");

    getchar();
    closesocket(clientSocket);
    socket_cleanup();
    printf("Client shutdown complete.\n");

    return 0;
}