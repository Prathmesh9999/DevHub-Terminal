#include "../core//file_download.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

int main(void)
{
    /*
     * Test file data
     */

    const unsigned char testData[] = "Hello DevHub Chunk!";

    uint32_t fileSize = 1000;
    uint32_t chunkNumber = 2;
    uint32_t totalChunks = 5;
    uint32_t chunkSize = (uint32_t)strlen((const char *)testData);

    unsigned char buffer[1024];

    printf("INPUT totalChunks = %u\n", totalChunks);
    printf("INPUT chunkSize = %u\n", chunkSize);

    int result = file_download_chunk(fileSize, chunkNumber, totalChunks, testData, chunkSize, buffer, sizeof(buffer));

    if (result < 0)
    {
        printf("Chunk Test : Failed to build chunk\n");
        return 1;
    }
    printf("Chunk Test : Built chunk size = %d bytes\n", result);

    /*
     * Expected size:
     *
     * 16 bytes metadata
     * + chunk data
     */
    unsigned int expectedSize =
        16 + chunkSize;

    if ((unsigned int)result != expectedSize)
    {
        printf("Chunk Test : Wrong response size\n");
        return 1;
    }

    /*
     * Decode file size
     */
    uint32_t decodedFileSize =
        ((uint32_t)buffer[0] << 24) |
        ((uint32_t)buffer[1] << 16) |
        ((uint32_t)buffer[2] << 8) |
        (uint32_t)buffer[3];

    /*
     * Decode chunk number
     */
    uint32_t decodedChunkNumber =
        ((uint32_t)buffer[4] << 24) |
        ((uint32_t)buffer[5] << 16) |
        ((uint32_t)buffer[6] << 8) |
        (uint32_t)buffer[7];

    /*
     * Decode total chunks
     */

    uint32_t decodedTotalChunks = ((uint32_t)buffer[8] << 24) | ((uint32_t)buffer[9] << 16) | ((uint32_t)buffer[10] << 8) | (uint32_t)buffer[11];

    /*
     * Decode chunk size
     */
    uint32_t decodedChunkSize =
        ((uint32_t)buffer[12] << 24) |
        ((uint32_t)buffer[13] << 16) |
        ((uint32_t)buffer[14] << 8) |
        (uint32_t)buffer[15];

    printf(
        "Chunk Test: File size = %u\n",
        decodedFileSize);

    printf(
        "Chunk Test: Chunk number = %u\n",
        decodedChunkNumber);

    printf(
        "Chunk Test: Total chunks = %u\n",
        decodedTotalChunks);

    printf(
        "Chunk Test: Chunk size = %u\n",
        decodedChunkSize);

    /*
     * Verify metadata
     */
    if (decodedFileSize != fileSize)
    {
        printf("Chunk Test: File size mismatch.\n");
        return 1;
    }

    if (decodedChunkNumber != chunkNumber)
    {
        printf("Chunk Test: Chunk number mismatch.\n");
        return 1;
    }

    if (decodedTotalChunks != totalChunks)
    {
        printf("Chunk Test: Total chunks mismatch.\n");
        return 1;
    }

    if (decodedChunkSize != chunkSize)
    {
        printf("Chunk Test: Chunk size mismatch.\n");
        return 1;
    }

    /*
     * Verify chunk data
     */

    if (memcmp(buffer + 16, testData, chunkSize) != 0)
    {
        printf("Chunk Test : Chunk data mismatch\n");
        return 1;
    }

    printf(
        "Chunk Test: Chunk data verified successfully.\n");

    printf(
        "Chunk Test: PASSED\n");

    return 0;
}