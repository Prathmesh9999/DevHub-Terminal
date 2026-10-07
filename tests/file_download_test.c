#include <stdio.h>
#include <string.h>

#include <stdint.h>

#include "../core/file_download.h"

int main(void)
{

    printf("====================================\n");
    printf("      DevHub File Download Test\n");
    printf("====================================\n\n");

    // ---------------------------------------------------------
    // FILE WE WANT TO DOWNLOAD
    // ---------------------------------------------------------

    const char *filename =
        "download_test.txt";

    // ---------------------------------------------------------
    // BUILD DOWNLOAD REQUEST PAYLOAD
    // ---------------------------------------------------------

    uint16_t filenameLength =
        (uint16_t)strlen(filename);

    unsigned char requestPayload[256];

    unsigned int requestSize = 0;

    /*
        Request format:

        [2 bytes] filename length
        [N bytes] filename
    */

    requestPayload[requestSize++] =
        (unsigned char)(filenameLength >> 8);

    requestPayload[requestSize++] =
        (unsigned char)(filenameLength & 0xFF);

    memcpy(
        requestPayload + requestSize,
        filename,
        filenameLength
    );

    requestSize += filenameLength;

    printf(
        "Requested file : %s\n",
        filename
    );

    printf(
        "Request size   : %u bytes\n\n",
        requestSize
    );

    // ---------------------------------------------------------
    // RESPONSE BUFFER
    // ---------------------------------------------------------

    unsigned char responseBuffer[1024];

    // ---------------------------------------------------------
    // CALL DOWNLOAD HANDLER
    // ---------------------------------------------------------

    int responseSize =
        file_download_handle(
            requestPayload,
            requestSize,
            responseBuffer,
            sizeof(responseBuffer)
        );

    if (responseSize < 0)
    {
        printf(
            "\nFILE DOWNLOAD HANDLER TEST FAILED.\n"
        );

        return 1;
    }

    printf(
        "\nResponse payload size : %d bytes\n",
        responseSize
    );

    // ---------------------------------------------------------
    // PARSE RESPONSE
    // ---------------------------------------------------------

    unsigned int offset = 0;

    if (responseSize < 6)
    {
        printf(
            "Response payload too small.\n"
        );

        return 1;
    }

    // ---------------------------------------------------------
    // READ FILENAME LENGTH
    // ---------------------------------------------------------

    uint16_t returnedFilenameLength =
        ((uint16_t)responseBuffer[offset] << 8) |
        responseBuffer[offset + 1];

    offset += 2;

    if (returnedFilenameLength == 0 ||
        returnedFilenameLength >= 256)
    {
        printf(
            "Invalid returned filename length.\n"
        );

        return 1;
    }

    // ---------------------------------------------------------
    // READ FILENAME
    // ---------------------------------------------------------

    char returnedFilename[256];

    memcpy(
        returnedFilename,
        responseBuffer + offset,
        returnedFilenameLength
    );

    returnedFilename[
        returnedFilenameLength
    ] = '\0';

    offset += returnedFilenameLength;

    printf(
        "Returned filename : %s\n",
        returnedFilename
    );

    // ---------------------------------------------------------
    // READ FILE SIZE
    // ---------------------------------------------------------

    uint32_t returnedFileSize =
        ((uint32_t)responseBuffer[offset] << 24) |
        ((uint32_t)responseBuffer[offset + 1] << 16) |
        ((uint32_t)responseBuffer[offset + 2] << 8) |
        (uint32_t)responseBuffer[offset + 3];

    offset += 4;

    printf(
        "Returned file size : %u bytes\n",
        returnedFileSize
    );

    // ---------------------------------------------------------
    // VALIDATE RESPONSE SIZE
    // ---------------------------------------------------------

    if (offset + returnedFileSize !=
        (unsigned int)responseSize)
    {
        printf(
            "Response file size mismatch.\n"
        );

        return 1;
    }

    // ---------------------------------------------------------
    // EXPECTED FILE CONTENT
    // ---------------------------------------------------------

    const char *expectedData =
        "This file came from the DevHub server.\n";

    unsigned int expectedSize =
        (unsigned int)strlen(expectedData);

    // ---------------------------------------------------------
    // COMPARE FILE DATA
    // ---------------------------------------------------------

    if (returnedFileSize != expectedSize)
    {
        printf(
            "Returned file size does not match expected size.\n"
        );

        return 1;
    }

    if (memcmp(
            responseBuffer + offset,
            expectedData,
            expectedSize) != 0)
    {
        printf(
            "Returned file content does not match.\n"
        );

        return 1;
    }

    printf(
        "Returned file content matches original.\n"
    );

    printf(
        "\nFILE DOWNLOAD HANDLER TEST PASSED.\n"
    );

    printf("\n====================================\n");
    printf("      File Download Test Complete\n");
    printf("====================================\n");

    return 0;
}