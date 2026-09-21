#include "file_handler.h"
#include "../networking/protocol.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

int file_upload_handle(const unsigned char *payload, unsigned int payloadLength)
{
    if (payload == NULL)
    {
        printf("File Handler : Invalid payload\n");
        return 0;
    }
    if (payloadLength < 6)
    {
        printf("File Handler : Payload too small\n");
        return 0;
    }
    /*
        Payload format:

        [2 bytes] filename length
        [N bytes] filename
        [4 bytes] file size
        [N bytes] file data
    */
    unsigned int offset = 0;

    uint16_t filenameLength = ((uint16_t)payload[offset] << 8) | payload[offset + 1];

    offset += 2;
    if (filenameLength == 0)
    {
        printf("File Handler : Empty filename\n");
        return 0;
    }
    if (filenameLength >= 256)
    {
        printf("File Handler : Filename too long\n");
        return 0;

        // ---------------------------------------------------------
        // CHECK THAT FILENAME EXISTS INSIDE PAYLOAD
        // ---------------------------------------------------------
    }
    if (offset + filenameLength + 4 > payloadLength)
    {
        printf("File Handler : Invalid filename section\n");
        return 0;
    }
    char filename[256];

    memcpy(filename, payload + offset, filenameLength);
    filename[filenameLength] = '\0';

    offset += filenameLength;

    // ---------------------------------------------------------
    // READ FILE SIZE
    // ---------------------------------------------------------

    uint32_t fileSize = ((uint32_t)payload[offset] << 24) | ((uint32_t)payload[offset + 1] << 16) | ((uint32_t)payload[offset + 2] << 8) | (uint32_t)payload[offset + 3];

    offset += 4;

    printf("File Handler : Filename = %s\n,", filename);
    printf("File Handler : File size = %u bytes\n,", fileSize);

    // ---------------------------------------------------------
    // VALIDATE FILE DATA SIZE
    // ---------------------------------------------------------

    if (offset + fileSize != payloadLength)
    {
        printf("File Handler : File data size mismatch\n");
        return 0;
    }

    // ---------------------------------------------------------
    // OPEN FILE
    // ---------------------------------------------------------

    char outputPath[512];

    snprintf(
        outputPath,
        sizeof(outputPath),
        "workspace/%s",
        filename);

    FILE *file =
        fopen(outputPath, "wb");

    if (file == NULL)
    {
        printf("File Handler : Failed to create file\n");
        return 0;
    }

    //----------------------------------------------------------
    // WRITE FILE DATA

    size_t bytesWritten = fwrite(payload + offset, 1, fileSize, file);
    fclose(file);

    if (bytesWritten != fileSize)
    {
        printf("File Handler : Failed to write complete file.\n");
        return 0;
    }
    printf(
        "File Handler: File uploaded successfully.\n");

    printf(
        "File Handler: Saved as %s\n",
        outputPath);
    return 1;
}