#ifndef DEVHUB_AUTH_H
#define DEVHUB_AUTH_H

int auth_handle(const unsigned char *payload, unsigned int payloadLength);
int auth_get_username(const unsigned char *payload,unsigned int payloadLength,char *username,unsigned int usernameSize);
int auth_build_payload(const char *username, const char *password, unsigned char *buffer, unsigned int bufferSize);
#endif