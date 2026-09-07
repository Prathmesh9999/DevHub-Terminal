#include "auth.h"

#include <stdio.h>
#include <string.h>

int auth_handle(const unsigned char *payload, unsigned int payloadLength)
{
    if (payload == NULL)
    {
        return 0;
    }
    /*
        Payload format:

        [1 byte username length]
        [username]
        [1 byte password length]
        [password]
    */
    //    char credentials[256];
    //    if(payloadLength>=sizeof(credentials))return 0;

    //    memcpy(credentials,payload,payloadLength);

    //    credentials[payloadLength]='\0';

    //    const char* separator=strchr(credentials,':');
    //    if(separator==NULL){
    //     printf("\nAuth handler : Invalid credentials format\n");
    //     return 0;
    //    }

    //    size_t usernameLength =separator-credentials;

    if (payloadLength < 2)
    {
        printf("Auth Handler : Invalid payload\n");
        return 0;
    }
    unsigned int offset = 0;
    unsigned int usernameLength = payload[offset++];
    if (usernameLength == 0)
    {
        printf("\nAuth Handler : Empty username\n");
        return 0;
    }
    if (offset + usernameLength >= payloadLength)
    {
        printf("Auth Handler : Invalid username length\n");
        return 0;
    }

    char username[128];
    if (usernameLength >= sizeof(username))
    {
        printf("Auth Handler : Username too long\n");
        return 0;
    }

    memcpy(username, payload + offset, usernameLength);

    username[usernameLength] = '\0';

    offset += usernameLength;

    // PASSWORD LENGTH

    if (offset >= payloadLength)
    {
        printf("\nAuth Handler : Missing Password length\n");
        return 0;
    }
    unsigned int passwordLength =
        payload[offset++];

    if (passwordLength == 0)
    {
        printf("Auth Handler: Empty password\n");
        return 0;
    }

    if(offset+passwordLength>payloadLength){
        printf("\nAuth Handler : Invalid password length\n");
        return 0;
    }
    char password[128];

    if(passwordLength>=sizeof(password)){
        printf("\nAuth Handler : Password too long\n");
        return 0;
    }

    memcpy(password,payload+offset,passwordLength);

    password[passwordLength]='\0';
    printf("Auth Handler : Username = %s\n",username);

    /*
        Temporary credentials for DevHub v1.
    */

    if(strcmp(username,"praxx")==0&&strcmp(password,"devhub999")==0){
        printf("\nAuth Handler : Authentication SUCCESS\n");
        return 1;
    }


    printf("\nAuth Handler : Authentication FAILED\n");
    return 0;
}

int auth_build_payload(const char* username,const char* password,unsigned char* buffer,unsigned int bufferSize){
    if(username==NULL||password==NULL||buffer==NULL){
        return -1;
    }

    unsigned int usernameLength=(unsigned int)strlen(username);
    unsigned int passwordLength=(unsigned int)strlen(password);

    if(usernameLength==0||passwordLength==0)return -1;

    if(usernameLength>255||passwordLength>255)return -1;

    unsigned int requiredSize=1+usernameLength+1+passwordLength;

    if(bufferSize<requiredSize)return -1;
    unsigned int offset=0;
    buffer[offset++]=(unsigned char )usernameLength;
    memcpy(buffer+offset,username,usernameLength);
    offset+=usernameLength;
    buffer[offset++]=(unsigned char)passwordLength;
    memcpy(buffer+offset,password,passwordLength);

    offset+=passwordLength;

    return (int)offset;
}

int auth_get_username(const unsigned char *payload,unsigned int payloadLength,char *username,unsigned int usernameSize)
{
    if (payload == NULL ||
        username == NULL ||
        usernameSize == 0)
    {
        return 0;
    }

    if (payloadLength < 2)
    {
        return 0;
    }

    unsigned int offset = 0;

    unsigned int usernameLength =
        payload[offset++];

    if (usernameLength == 0)
    {
        return 0;
    }

    if (usernameLength >= usernameSize)
    {
        return 0;
    }

    if (offset + usernameLength >= payloadLength)
    {
        return 0;
    }

    memcpy(
        username,
        payload + offset,
        usernameLength
    );

    username[usernameLength] = '\0';

    return 1;
}