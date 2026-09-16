#include"client_manager.h"

static ClientConnection clients[DEVHUB_MAX_CLIENTS];

void client_manager_init(){
    for(int i=0;i<DEVHUB_MAX_CLIENTS;i++){
        clients[i].active=0;
    }
}

int client_manager_add(SOCKET socket){
    for(int i=0;i<DEVHUB_MAX_CLIENTS;i++){
        if(!clients[i].active){
            session_init(&clients[i].session,socket);
                clients[i].active=1;
            return i;
        }
    }
    return -1;
}

void client_manager_remove(int index){
    if(index<0 || index>=DEVHUB_MAX_CLIENTS)return;

    clients[index].active=0;
}

ClientConnection * client_manager_get(int index){
    if(index<0 || index>=DEVHUB_MAX_CLIENTS)return NULL;
    if(!clients[index].active)return NULL;

    return &clients[index];
}

