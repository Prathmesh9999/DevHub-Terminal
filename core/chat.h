#ifndef DEVHUB_CHAT_H
#define DEVHUB_CHAT_H

int chat_handle(const unsigned char* payload,unsigned int payloadLength);
int chat_build_response(unsigned char* buffer,unsigned int bufferSize);
#endif