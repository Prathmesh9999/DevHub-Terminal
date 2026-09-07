#include<stdio.h>

#include "../core/session.h"

int main(){
    ClientSession session;
    printf("Testing session module...\n");

    session_init(&session,(SOCKET)1234);
    printf("Initial authentication : %s\n",session_is_authenticated(&session)?"AUTHENTICATED":"NOT AUTHENTICATED");

    int result=session_authenticate(&session,"PRAXX");

    printf("Authentication result :  %s\n",result?"SUCCESS":"FAILED");
    printf("Authenticated : %s\n",session_is_authenticated(&session)?"YES":"NO");
    printf("Username: %s\n",session.username);
    session_logout(&session);
    printf("After logout : %s\n",session_is_authenticated(&session)?"AUTHENTICATED":"NOT AUTHENTICATED");

    return 0;
}