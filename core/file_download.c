#include "file_download.h"
#include <direct.h>
#include "../networking/protocol.h"
#include "../networking/socket.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

int file_download_handle(const unsigned char *payload, unsigned int payloadLength, unsigned char *responseBuffer, unsigned int responseBufferSize)
{

    if (payload == NULL)
    {
        printf("File Download : Invalid request payload\n");
        return -1;
    }
    if (responseBuffer == NULL)
    {
        printf("File Download : Invalid response buffer\n");
        return -1;
    }
    /*
       Download request format:

       [2 bytes] filename length
       [N bytes] filename
   */

    if (payloadLength < 2)
    {
        printf("File Download : Request too small\n");
        return -1;
    }

    unsigned int offset = 0;

    uint16_t filenameLength = ((uint16_t)payload[offset] << 8) | payload[offset + 1];

    offset += 2;

    if (filenameLength == 0)
    {
        printf("File Download : Empty filename\n");
        return -1;
    }

    if (offset + filenameLength != payloadLength)
    {
        printf("File Download : Invalid filename section\n");
        return -1;
    }

    char filename[256];

    memcpy(filename, payload + offset, filenameLength);

    filename[filenameLength] = '\0';
    if (strstr(filename, "..") != NULL ||
        strchr(filename, '/') != NULL ||
        strchr(filename, '\\') != NULL)
    {
        printf(
            "File Download : Invalid filename.\n");

        return -1;
    }

    printf("File Download : Request file = %s\n", filename);

    char filePath[512];

    // BUILD SERVER FILE PATH
    snprintf(filePath, sizeof(filePath), "workspace/%s", filename);
    char currentDirectory[512];

    if (_getcwd(
            currentDirectory,
            sizeof(currentDirectory)) != NULL)
    {
        printf(
            "File Download: Current directory = %s\n",
            currentDirectory);
    }

    printf(
        "File Download: Opening path = %s\n",
        filePath);

    // OPEN FILE

    FILE *file = fopen(filePath, "rb");

    if (file == NULL)
    {
        printf("File Download : File not found\n");
        return -1;
    }

    fseek(file, 0, SEEK_END);

    long fileSizeLong = ftell(file);

    fseek(file, 0, SEEK_SET);

    if (fileSizeLong < 0)
    {
        printf("File Download : Failed to get file size\n");
        fclose(file);
        return -1;
    }

    uint32_t fileSize = (uint32_t)fileSizeLong;

    printf("File Download : File size =%u bytes\n", fileSize);

    // CALCULATE RESPONSE SIZE

    unsigned int requiredSize = 2 + filenameLength + 4 + fileSize;

    if (requiredSize > responseBufferSize)
    {
        printf("File Download : File too large for current buffer\n");
        fclose(file);
        return -1;
    }

    // BUILD RESPONSE PAYLOAD

    unsigned int responseOffset = 0;
    responseBuffer[responseOffset++] = (unsigned char)(filenameLength >> 8);

    responseBuffer[responseOffset++] = (unsigned char)(filenameLength & 0xFF);

    // filename
    memcpy(responseBuffer + responseOffset, filename, filenameLength);

    responseOffset += filenameLength;

    responseBuffer[responseOffset++] = (unsigned char)(fileSize >> 24);
    responseBuffer[responseOffset++] = (unsigned char)(fileSize >> 16);
    responseBuffer[responseOffset++] = (unsigned char)(fileSize >> 8);
    responseBuffer[responseOffset++] = (unsigned char)(fileSize);

    // READ FILE DATA

    size_t bytesRead = fread(responseBuffer + responseOffset, 1, fileSize, file);
    fclose(file);
    if (bytesRead != fileSize)
    {
        printf("File Download : Failed to read complete file\n");
        return -1;
    }
    responseOffset += (unsigned int)bytesRead;

    printf("File Download : File preapared successfully\n");

    return (int)responseOffset;
}

int file_download_chunk(uint32_t fileSize, uint32_t chunkNumber, uint32_t totalChunks, const unsigned char *chunkData, uint32_t chunkSize, unsigned char *responseBuffer, unsigned int responseBufferSize)
{

    if (responseBuffer == NULL)
    {
        printf(
            "File Download : Invalid chunk response buffer\n");

        return -1;
    }

    if (chunkData == NULL && chunkSize > 0)
    {
        printf(
            "File Download : Invalid chunk data\n");

        return -1;
    }

    unsigned int requiredSize = 16 + chunkSize;

    if (requiredSize > responseBufferSize)
    {
        printf(
            "File Download : Chunk too large for buffer\n");

        return -1;
    }

    unsigned int offset = 0;

    /* File size - 4 bytes */
    responseBuffer[offset++] = (unsigned char)((fileSize >> 24) & 0xFF);
    responseBuffer[offset++] = (unsigned char)((fileSize >> 16) & 0xFF);
    responseBuffer[offset++] = (unsigned char)((fileSize >> 8) & 0xFF);
    responseBuffer[offset++] = (unsigned char)(fileSize & 0xFF);

    /* Chunk number - 4 bytes */
    responseBuffer[offset++] = (unsigned char)((chunkNumber >> 24) & 0xFF);
    responseBuffer[offset++] = (unsigned char)((chunkNumber >> 16) & 0xFF);
    responseBuffer[offset++] = (unsigned char)((chunkNumber >> 8) & 0xFF);
    responseBuffer[offset++] = (unsigned char)(chunkNumber & 0xFF);

    /* Total chunks - 4 bytes */
    responseBuffer[offset++] = (unsigned char)((totalChunks >> 24) & 0xFF);
    responseBuffer[offset++] = (unsigned char)((totalChunks >> 16) & 0xFF);
    responseBuffer[offset++] = (unsigned char)((totalChunks >> 8) & 0xFF);
    responseBuffer[offset++] = (unsigned char)(totalChunks & 0xFF);

    /* Chunk size - 4 bytes */
    responseBuffer[offset++] = (unsigned char)((chunkSize >> 24) & 0xFF);
    responseBuffer[offset++] = (unsigned char)((chunkSize >> 16) & 0xFF);
    responseBuffer[offset++] = (unsigned char)((chunkSize >> 8) & 0xFF);
    responseBuffer[offset++] = (unsigned char)(chunkSize & 0xFF);
    /*
     * Copy actual file data
     */
    if (chunkSize > 0)
    {
        memcpy(
            responseBuffer + offset,
            chunkData,
            chunkSize);

        offset += chunkSize;
    }
    printf(
        "DEBUG: totalChunks input = %u\n",
        totalChunks);

    printf(
        "DEBUG: totalChunks bytes = %02X %02X %02X %02X\n",
        responseBuffer[8],
        responseBuffer[9],
        responseBuffer[10],
        responseBuffer[11]);

    return (int)offset;
}

int file_download_send_chunks(
    SOCKET clientSocket,
    const char *filename)
{
    if (filename == NULL)
    {
        printf(
            "File Download : Invalid filename\n");

        return -1;
    }

    char filePath[512];

    snprintf(
        filePath,
        sizeof(filePath),
        "workspace/%s",
        filename);

    printf(
        "File Download : Chunked download path = %s\n",
        filePath);

    FILE *file = fopen(filePath, "rb");

    if (file == NULL)
    {
        printf(
            "File Download : Failed to open file\n");

        return -1;
    }

    /*
     * Get file size
     */
    if (fseek(file, 0, SEEK_END) != 0)
    {
        printf(
            "File Download : Failed to seek file\n");

        fclose(file);
        return -1;
    }

    long fileSizeLong = ftell(file);

    if (fileSizeLong < 0)
    {
        printf(
            "File Download : Failed to get file size\n");

        fclose(file);
        return -1;
    }

    if (fseek(file, 0, SEEK_SET) != 0)
    {
        printf(
            "File Download : Failed to reset file position\n");

        fclose(file);
        return -1;
    }

    uint32_t fileSize =
        (uint32_t)fileSizeLong;

    /*
     * Calculate total chunks
     */
    uint32_t totalChunks =
        (fileSize + DEVHUB_FILE_CHUNK_SIZE - 1) / DEVHUB_FILE_CHUNK_SIZE;

    printf(
        "File Download : File size = %u bytes\n",
        fileSize);

    printf(
        "File Download : Chunk size = %u bytes\n",
        DEVHUB_FILE_CHUNK_SIZE);

    printf(
        "File Download : Total chunks = %u\n",
        totalChunks);

    /*
     * Read file chunk-by-chunk
     */
    unsigned char chunkData[DEVHUB_FILE_CHUNK_SIZE];

    uint32_t chunkNumber = 0;

    while (1)
    {
        size_t bytesRead =
            fread(
                chunkData,
                1,
                DEVHUB_FILE_CHUNK_SIZE,
                file);

        if (bytesRead == 0)
        {
            break;
        }

        printf(
            "File Download : Read chunk %u (%zu bytes)\n",
            chunkNumber,
            bytesRead);

        /*
         * Build chunk payload
         */
        unsigned char chunkPayload[1024];

        int chunkPayloadSize =
            file_download_chunk(
                fileSize,
                chunkNumber,
                totalChunks,
                chunkData,
                (uint32_t)bytesRead,
                chunkPayload,
                sizeof(chunkPayload));

        if (chunkPayloadSize < 0)
        {
            printf(
                "File Download : Failed to build chunk %u\n",
                chunkNumber);

            fclose(file);
            return -1;
        }

        /*
         * Build DevHub packet
         */
        DevHubHeader header;

        header.version =
            DEVHUB_PROTOCOL_VERSION;

        header.type =
            DEVHUB_MSG_FILE_DOWNLOAD_CHUNK;

        header.payloadLength =
            (uint32_t)chunkPayloadSize;

        unsigned char packet[DEVHUB_HEADER_SIZE + 1024];

        int packetSize =
            protocol_build_packet(
                &header,
                chunkPayload,
                packet);

        if (packetSize < 0)
        {
            printf(
                "File Download : Failed to build packet for chunk %u\n",
                chunkNumber);

            fclose(file);
            return -1;
        }

        /*
         * Send packet
         */
        int sent =
            socket_send(
                clientSocket,
                (const char *)packet,
                packetSize);

        if (sent != packetSize)
        {
            printf(
                "File Download : Failed to send chunk %u\n",
                chunkNumber);

            fclose(file);
            return -1;
        }

        printf(
            "File Download : Chunk %u sent successfully (%d bytes)\n",
            chunkNumber,
            packetSize);

        chunkNumber++;
    }

    fclose(file);

    printf(
        "File Download : Finished reading chunks\n");

    return 0;
}