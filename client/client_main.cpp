#include "../networking/protocol.h"
#include "../networking/socket.h"
#include "../core/auth.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

extern "C"
{
#include "logger.h"
}

#include "Config.hpp"

/*
 * =========================================================
 * RECEIVE CHUNKED FILE DOWNLOAD
 * =========================================================
 *
 * Chunk payload:
 *
 * [4 bytes] File size
 * [4 bytes] Chunk number
 * [4 bytes] Total chunks
 * [4 bytes] Chunk size
 * [N bytes] Chunk data
 *
 */

int receive_download_chunks(
    SOCKET socket,
    const char *outputPath)
{
    if (outputPath == NULL)
    {
        printf("Client : Invalid output path.\n");
        return 0;
    }

    FILE *outputFile =
        fopen(outputPath, "wb");

    if (outputFile == NULL)
    {
        printf(
            "Client : Failed to create output file.\n");

        return 0;
    }

    uint32_t expectedTotalChunks = 0;
    uint32_t expectedFileSize = 0;
    uint32_t receivedChunks = 0;
    uint32_t receivedBytes = 0;

    while (1)
    {
        /*
         * -----------------------------------------------------
         * Receive DevHub header
         * -----------------------------------------------------
         */

        DevHubHeader header;

        int headerResult =
            protocol_receive_header(
                socket,
                &header);

        if (headerResult <= 0)
        {
            printf(
                "Client : Failed to receive chunk header.\n");

            fclose(outputFile);
            return 0;
        }

        /*
         * Make sure this is a chunk packet
         */

        if (header.type !=
            DEVHUB_MSG_FILE_DOWNLOAD_CHUNK)
        {
            printf(
                "Client : Unexpected message type %u.\n",
                header.type);

            fclose(outputFile);
            return 0;
        }

        /*
         * -----------------------------------------------------
         * Validate payload size
         * -----------------------------------------------------
         */

        if (header.payloadLength < 16)
        {
            printf(
                "Client : Invalid chunk payload.\n");

            fclose(outputFile);
            return 0;
        }

        if (header.payloadLength > 1024)
        {
            printf(
                "Client : Chunk payload too large.\n");

            fclose(outputFile);
            return 0;
        }

        /*
         * -----------------------------------------------------
         * Receive chunk payload
         * -----------------------------------------------------
         */

        unsigned char payload[1024];

        int payloadResult =
            protocol_receive_payload(
                socket,
                &header,
                payload);

        if (payloadResult <= 0)
        {
            printf(
                "Client : Failed to receive chunk payload.\n");

            fclose(outputFile);
            return 0;
        }

        /*
         * -----------------------------------------------------
         * Decode metadata
         * -----------------------------------------------------
         */

        uint32_t fileSize =
            ((uint32_t)payload[0] << 24) |
            ((uint32_t)payload[1] << 16) |
            ((uint32_t)payload[2] << 8) |
            (uint32_t)payload[3];

        uint32_t chunkNumber =
            ((uint32_t)payload[4] << 24) |
            ((uint32_t)payload[5] << 16) |
            ((uint32_t)payload[6] << 8) |
            (uint32_t)payload[7];

        uint32_t totalChunks =
            ((uint32_t)payload[8] << 24) |
            ((uint32_t)payload[9] << 16) |
            ((uint32_t)payload[10] << 8) |
            (uint32_t)payload[11];

        uint32_t chunkSize =
            ((uint32_t)payload[12] << 24) |
            ((uint32_t)payload[13] << 16) |
            ((uint32_t)payload[14] << 8) |
            (uint32_t)payload[15];

        printf(
            "Client : Received chunk %u/%u (%u bytes)\n",
            chunkNumber + 1,
            totalChunks,
            chunkSize);

        /*
         * -----------------------------------------------------
         * Validate total chunks
         * -----------------------------------------------------
         */

        if (totalChunks == 0)
        {
            printf(
                "Client : Invalid total chunk count.\n");

            fclose(outputFile);
            return 0;
        }

        /*
         * -----------------------------------------------------
         * Validate chunk number
         * -----------------------------------------------------
         */

        if (chunkNumber >= totalChunks)
        {
            printf("Client : Invalid chunk number.\n");
            fclose(outputFile);
            return 0;
        }

        if (chunkNumber != receivedChunks)
        {
            printf("Client : Unexpected chunk number.\n");
            fclose(outputFile);
            return 0;
        }

        /*
         * -----------------------------------------------------
         * Validate chunk size
         * -----------------------------------------------------
         */

        if (chunkSize >
            header.payloadLength - 16)
        {
            printf(
                "Client : Invalid chunk size.\n");

            fclose(outputFile);
            return 0;
        }

        if (chunkSize !=
            header.payloadLength - 16)
        {
            printf(
                "Client : Chunk payload size mismatch.\n");

            fclose(outputFile);
            return 0;
        }

        /*
         * -----------------------------------------------------
         * First chunk establishes expected metadata
         * -----------------------------------------------------
         */

        if (receivedChunks == 0)
        {
            expectedFileSize = fileSize;
            expectedTotalChunks = totalChunks;
        }
        else
        {
            /*
             * Every chunk must describe the same file
             */

            if (fileSize != expectedFileSize ||
                totalChunks != expectedTotalChunks)
            {
                printf(
                    "Client : Chunk metadata mismatch.\n");

                fclose(outputFile);
                return 0;
            }
        }

        /*
         * -----------------------------------------------------
         * Write chunk data
         * -----------------------------------------------------
         */

        size_t bytesWritten =
            fwrite(
                payload + 16,
                1,
                chunkSize,
                outputFile);

        if (bytesWritten != chunkSize)
        {
            printf(
                "Client : Failed to write chunk.\n");

            fclose(outputFile);
            return 0;
        }

        receivedBytes += chunkSize;
        receivedChunks++;

        /*
         * -----------------------------------------------------
         * Check whether this was the final chunk
         * -----------------------------------------------------
         */

        if (receivedChunks == expectedTotalChunks)
        {
            break;
        }
    }

    fclose(outputFile);

    /*
     * ---------------------------------------------------------
     * Final file-size verification
     * ---------------------------------------------------------
     */

    if (receivedBytes != expectedFileSize)
    {
        printf(
            "Client : File size mismatch.\n");

        printf(
            "Client : Expected %u bytes, received %u bytes.\n",
            expectedFileSize,
            receivedBytes);

        return 0;
    }

    printf(
        "Client : Download completed successfully.\n");

    printf(
        "Client : Received %u bytes in %u chunks.\n",
        receivedBytes,
        receivedChunks);

    printf(
        "Client : Saved as %s\n",
        outputPath);

    return 1;
}

/*
 * =========================================================
 * MAIN
 * =========================================================
 */

int main(int argc, char *argv[])
{
    /*
     * ---------------------------------------------------------
     * Download command-line arguments
     * ---------------------------------------------------------
     *
     * Example:
     *
     * .\devhub_client.exe sample.cpp downloaded_A.cpp
     *
     */

    const char *downloadFileName =
        "sample.cpp";

    const char *downloadOutputPath =
        "downloaded_sample.cpp";

    if (argc >= 2)
    {
        downloadFileName = argv[1];
    }

    if (argc >= 3)
    {
        downloadOutputPath = argv[2];
    }

    printf(
        "Starting DevHub TCP client...\n");

    /*
     * =========================================================
     * 1. INITIALIZE WINSOCK
     * =========================================================
     */

    if (!socket_initialize())
    {
        printf(
            "FAILED TO INITIALIZE WINSOCK\n");

        return 1;
    }

    /*
     * =========================================================
     * 2. CREATE TCP SOCKET
     * =========================================================
     */

    SOCKET clientSocket =
        socket_create_tcp();

    if (clientSocket == INVALID_SOCKET)
    {
        printf(
            "FAILED TO CREATE TCP SOCKET\n");

        socket_cleanup();

        return 1;
    }

    printf(
        "TCP socket created.\n");

    /*
     * =========================================================
     * 3. CONNECT TO DEVHUB SERVER
     * =========================================================
     */

    printf(
        "Connecting to 127.0.0.1:5000...\n");

    if (!socket_connect(
            clientSocket,
            "127.0.0.1",
            5000))
    {
        printf(
            "FAILED TO CONNECT TO DEVHUB SERVER\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    printf(
        "Connected to DevHub server!\n");

    /*
     * =========================================================
     * AUTHENTICATION
     * =========================================================
     */

    const char *username =
        "praxx";

    const char *password =
        "devhub999";

    /*
     * =========================================================
     * 4. BUILD AUTHENTICATION PAYLOAD
     * =========================================================
     */

    unsigned char authPayload[256];

    int authPayloadSize =
        auth_build_payload(
            username,
            password,
            authPayload,
            sizeof(authPayload));

    if (authPayloadSize < 0)
    {
        printf(
            "FAILED TO BUILD AUTH PAYLOAD\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    printf(
        "Auth payload built. Size: %d bytes\n",
        authPayloadSize);

    /*
     * =========================================================
     * 5. CREATE AUTH HEADER
     * =========================================================
     */

    DevHubHeader authHeader;

    authHeader.version =
        DEVHUB_PROTOCOL_VERSION;

    authHeader.type =
        DEVHUB_MSG_AUTH;

    authHeader.payloadLength =
        (uint32_t)authPayloadSize;

    /*
     * =========================================================
     * 6. BUILD AUTH PACKET
     * =========================================================
     */

    unsigned char authPacket[DEVHUB_HEADER_SIZE + 256];

    int authPacketSize =
        protocol_build_packet(
            &authHeader,
            authPayload,
            authPacket);

    if (authPacketSize < 0)
    {
        printf(
            "FAILED TO BUILD AUTH PACKET\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    printf(
        "Auth packet built. Size: %d bytes\n",
        authPacketSize);

    /*
     * =========================================================
     * 7. SEND AUTH PACKET
     * =========================================================
     */

    int bytesSent =
        socket_send(
            clientSocket,
            (const char *)authPacket,
            authPacketSize);

    if (bytesSent != authPacketSize)
    {
        printf(
            "FAILED TO SEND AUTH PACKET\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    printf(
        "AUTH packet sent.\n");

    /*
     * =========================================================
     * RECEIVE SERVER AUTH RESPONSE
     * =========================================================
     */

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

    printf(
        "\nServer response received.\n");

    printf(
        "Response version: %u\n",
        responseHeader.version);

    printf(
        "Response type: %u\n",
        responseHeader.type);

    printf(
        "Response payload length: %u\n",
        responseHeader.payloadLength);

    /*
     * =========================================================
     * RECEIVE AUTH RESPONSE PAYLOAD
     * =========================================================
     */

    unsigned char responsePayload[1024];

    if (responseHeader.payloadLength >=
        sizeof(responsePayload))
    {
        printf(
            "SERVER RESPONSE PAYLOAD TOO LARGE\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

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

    printf(
        "Server response: %s\n",
        responsePayload);

    printf(
        "\nClient authenticated successfully.\n");

    /*
     * =========================================================
     * CHAT TEST
     * =========================================================
     */

    printf(
        "\nSending CHAT message...\n");

    const char *chatMessage =
        "Hello from DevHub Client";

    DevHubHeader chatHeader;

    chatHeader.version =
        DEVHUB_PROTOCOL_VERSION;

    chatHeader.type =
        DEVHUB_MSG_CHAT;

    chatHeader.payloadLength =
        (uint32_t)strlen(chatMessage);

    unsigned char chatPacket[DEVHUB_HEADER_SIZE +
                             DEVHUB_MAX_PAYLOAD_SIZE];

    int chatPacketSize =
        protocol_build_packet(
            &chatHeader,
            (const unsigned char *)chatMessage,
            chatPacket);

    if (chatPacketSize < 0)
    {
        printf(
            "FAILED TO BUILD CHAT PACKET\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    printf(
        "CHAT packet built. Size: %d bytes\n",
        chatPacketSize);

    int chatByteSent =
        socket_send(
            clientSocket,
            (const char *)chatPacket,
            chatPacketSize);

    if (chatByteSent != chatPacketSize)
    {
        printf(
            "FAILED TO SEND CHAT PACKET\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    printf(
        "CHAT packet sent successfully.\n");

    printf(
        "\nWaiting for CHAT response...\n");

    DevHubHeader chatResponseHeader;

    int chatResponseHeaderResult =
        protocol_receive_header(
            clientSocket,
            &chatResponseHeader);

    if (chatResponseHeaderResult != 1)
    {
        printf(
            "FAILED TO RECEIVE CHAT RESPONSE HEADER\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    printf(
        "CHAT response header received.\n");

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
        printf(
            "CHAT RESPONSE PAYLOAD TOO LARGE\n");

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
        printf(
            "FAILED TO RECEIVE CHAT RESPONSE PAYLOAD\n");

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
        printf(
            "CHAT end-to-end test PASSED.\n");
    }
    else
    {
        printf(
            "CHAT end-to-end test FAILED.\n");
    }

    /*
     * =========================================================
     * FILE UPLOAD TEST
     * =========================================================
     */

    printf(
        "\nStarting FILE_UPLOAD test...\n");

    const char *filePath =
        "sample.cpp";

    const char *fileName =
        "sample.cpp";

    /*
     * OPEN FILE
     */

    FILE *file =
        fopen(filePath, "rb");

    if (file == NULL)
    {
        printf(
            "FAILED TO OPEN FILE FOR UPLOAD\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    /*
     * GET FILE SIZE
     */

    fseek(file, 0, SEEK_END);

    long fileSizeLong =
        ftell(file);

    fseek(file, 0, SEEK_SET);

    if (fileSizeLong < 0)
    {
        printf(
            "FAILED TO GET FILE SIZE\n");

        fclose(file);
        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    uint32_t fileSize =
        (uint32_t)fileSizeLong;

    printf(
        "File name : %s\n",
        fileName);

    printf(
        "File Size : %u bytes\n",
        fileSize);

    /*
     * READ FILE
     */

    unsigned char fileData[512];

    if (fileSize > sizeof(fileData))
    {
        printf(
            "FILE TOO LARGE FOR CURRENT TEST\n");

        fclose(file);
        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    size_t bytesRead =
        fread(
            fileData,
            1,
            fileSize,
            file);

    fclose(file);

    if (bytesRead != fileSize)
    {
        printf(
            "FAILED TO READ COMPLETE FILE\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    /*
     * BUILD FILE_UPLOAD PAYLOAD
     */

    uint16_t fileNameLength =
        (uint16_t)strlen(fileName);

    unsigned char uploadPayload[1024];

    unsigned int offset = 0;

    uploadPayload[offset++] =
        (unsigned char)(fileNameLength >> 8);

    uploadPayload[offset++] =
        (unsigned char)(fileNameLength & 0xFF);

    memcpy(
        uploadPayload + offset,
        fileName,
        fileNameLength);

    offset += fileNameLength;

    /*
     * File size - 4 bytes
     */

    uploadPayload[offset++] =
        (unsigned char)(fileSize >> 24);

    uploadPayload[offset++] =
        (unsigned char)(fileSize >> 16);

    uploadPayload[offset++] =
        (unsigned char)(fileSize >> 8);

    uploadPayload[offset++] =
        (unsigned char)(fileSize);

    /*
     * File data
     */

    memcpy(
        uploadPayload + offset,
        fileData,
        fileSize);

    offset += fileSize;

    printf(
        "FILE_UPLOAD payload size : %u bytes\n",
        offset);

    /*
     * BUILD DEVHUB PACKET
     */

    DevHubHeader uploadHeader;

    uploadHeader.version =
        DEVHUB_PROTOCOL_VERSION;

    uploadHeader.type =
        DEVHUB_MSG_FILE_UPLOAD;

    uploadHeader.payloadLength =
        (uint32_t)offset;

    unsigned char uploadPacket[DEVHUB_HEADER_SIZE + 1024];

    int uploadPacketSize =
        protocol_build_packet(
            &uploadHeader,
            uploadPayload,
            uploadPacket);

    if (uploadPacketSize < 0)
    {
        printf(
            "FAILED TO BUILD FILE_UPLOAD PACKET\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    printf(
        "FILE UPLOAD packet size : %d bytes\n",
        uploadPacketSize);

    /*
     * SEND FILE_UPLOAD PACKET
     */

    int uploadBytesSent =
        socket_send(
            clientSocket,
            (const char *)uploadPacket,
            uploadPacketSize);

    if (uploadBytesSent != uploadPacketSize)
    {
        printf(
            "FAILED TO SEND FILE_UPLOAD PACKET\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    printf(
        "FILE_UPLOAD packet sent successfully\n");

    printf(
        "\nWaiting for FILE_UPLOAD response...\n");

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

    /*
     * =========================================================
     * FILE DOWNLOAD TEST
     * =========================================================
     */

    printf(
        "\nStarting FILE_DOWNLOAD test...\n");

    /*
     * BUILD DOWNLOAD PAYLOAD
     */

    uint16_t downloadFileNameLength =
        (uint16_t)strlen(downloadFileName);

    unsigned char downloadPayload[256];

    unsigned int downloadPayloadSize = 0;

    /*
     * FILENAME LENGTH
     */

    downloadPayload[downloadPayloadSize++] =
        (unsigned char)(downloadFileNameLength >> 8);

    downloadPayload[downloadPayloadSize++] =
        (unsigned char)(downloadFileNameLength & 0xFF);

    /*
     * FILENAME
     */

    memcpy(
        downloadPayload + downloadPayloadSize,
        downloadFileName,
        downloadFileNameLength);

    downloadPayloadSize +=
        downloadFileNameLength;

    printf(
        "Download file : %s\n",
        downloadFileName);

    printf(
        "Download request payload size : %u bytes\n",
        downloadPayloadSize);

    /*
     * BUILD DOWNLOAD HEADER
     */

    DevHubHeader downloadHeader;

    downloadHeader.version =
        DEVHUB_PROTOCOL_VERSION;

    downloadHeader.type =
        DEVHUB_MSG_FILE_DOWNLOAD;

    downloadHeader.payloadLength =
        (uint32_t)downloadPayloadSize;

    unsigned char downloadPacket[DEVHUB_HEADER_SIZE + 256];

    int downloadPacketSize =
        protocol_build_packet(
            &downloadHeader,
            downloadPayload,
            downloadPacket);

    if (downloadPacketSize < 0)
    {
        printf(
            "FAILED TO BUILD FILE_DOWNLOAD PACKET\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    printf(
        "FILE_DOWNLOAD packet size : %d bytes\n",
        downloadPacketSize);

    /*
     * SEND DOWNLOAD REQUEST
     */

    int downloadBytesSent =
        socket_send(
            clientSocket,
            (const char *)downloadPacket,
            downloadPacketSize);

    if (downloadBytesSent != downloadPacketSize)
    {
        printf(
            "FAILED TO SEND FILE_DOWNLOAD PACKET\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    printf(
        "FILE_DOWNLOAD request sent successfully.\n");

    /*
     * =========================================================
     * RECEIVE CHUNKED DOWNLOAD
     * =========================================================
     */

    printf(
        "\nWaiting for FILE_DOWNLOAD chunks...\n");

    int downloadResult =
        receive_download_chunks(
            clientSocket,
            downloadOutputPath);

    if (!downloadResult)
    {
        printf(
            "FILE_DOWNLOAD receive test FAILED.\n");

        closesocket(clientSocket);
        socket_cleanup();

        return 1;
    }

    printf(
        "FILE_DOWNLOAD receive test PASSED.\n");

    /*
     * =========================================================
     * KEEP CLIENT CONNECTED
     * =========================================================
     */

    printf(
        "\nClient is staying connected.\n");

    printf(
        "Press ENTER to disconnect this client...\n");

    getchar();

    closesocket(clientSocket);

    socket_cleanup();

    printf(
        "Client shutdown complete.\n");

    return 0;
}