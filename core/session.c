#include "session.h"
#include<string.h>

void session_init(ClientSession * session,SOCKET socket){
    if(session==NULL){
        return;
    }
    session->socket=socket;
    session->authenticated=0;
    session->username[0]='\0';
}

int session_authenticate(ClientSession * session,const char * username){
    if(session==NULL||username==NULL)return 0;
    strncpy(session->username,username,sizeof(session->username)-1);
    session->username[sizeof(session->username)-1]='\0';

    session->authenticated=1;
    return 1;
}

int session_is_authenticated(const ClientSession* session){
if(session==NULL)return 0;
return session->authenticated;
}

void session_logout(ClientSession* session){
    if(!session)return;

    session->authenticated=0;
    session->username[0]='\0';
}

