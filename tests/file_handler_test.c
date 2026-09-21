#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "../core/file_handler.h"

int main(void)
{
    printf("====================================\n");
    printf("       DevHub File Handler Test\n");
    printf("====================================\n\n");

    const char *filename =
        "test.txt";

    const char *fileData =
        "Hello DevHub!";

    uint16_t filenameLength =
        (uint16_t)strlen(filename);

    uint32_t fileSize =
        (uint32_t)strlen(fileData);

    unsigned char payload[256];

    unsigned int offset = 0;

    // ---------------------------------------------------------
    // FILENAME LENGTH
    // ---------------------------------------------------------

    payload[offset++] =
        (unsigned char)(filenameLength >> 8);

    payload[offset++] =
        (unsigned char)(filenameLength & 0xFF);

    // ---------------------------------------------------------
    // FILENAME
    // ---------------------------------------------------------

    memcpy(
        payload + offset,
        filename,
        filenameLength
    );

    offset += filenameLength;

    // ---------------------------------------------------------
    // FILE SIZE
    // ---------------------------------------------------------

    payload[offset++] =
        (unsigned char)(fileSize >> 24);

    payload[offset++] =
        (unsigned char)(fileSize >> 16);

    payload[offset++] =
        (unsigned char)(fileSize >> 8);

    payload[offset++] =
        (unsigned char)(fileSize);

    // ---------------------------------------------------------
    // FILE DATA
    // ---------------------------------------------------------

    memcpy(
        payload + offset,
        fileData,
        fileSize
    );

    offset += fileSize;

    printf(
        "Filename      : %s\n",
        filename
    );

    printf(
        "Filename size : %u bytes\n",
        filenameLength
    );

    printf(
        "File size     : %u bytes\n",
        fileSize
    );

    printf(
        "Payload size  : %u bytes\n\n",
        offset
    );

    // ---------------------------------------------------------
    // CALL FILE HANDLER
    // ---------------------------------------------------------

    int result =
        file_upload_handle(
            payload,
            offset
        );

    if (result)
    {
        printf(
            "\nFILE UPLOAD TEST PASSED.\n"
        );
    }
    else
    {
        printf(
            "\nFILE UPLOAD TEST FAILED.\n"
        );
    }

    printf("\n====================================\n");
    printf("       File Handler Test Complete\n");
    printf("====================================\n");

    return 0;
}