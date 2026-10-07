#ifndef DEVHUB_FILE_DOWNLOAD_H
#define DEVHUB_FILE_DOWNLOAD_H
#include<stdint.h>
#include "../networking/socket.h"

int file_download_handle(const unsigned char* payload,unsigned int payloadlength,unsigned char* responseBuffer, unsigned int responseBufferSize );
int file_download_chunk(uint32_t fileSize,uint32_t chunkNumber,uint32_t totalChunks,const unsigned char * chunkData,uint32_t chunkSize,unsigned char* responseBuffer,unsigned int responseBufferSize );
int file_download_send_chunks(SOCKET clientSocket,const char* filename);
#endif