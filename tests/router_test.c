#include "chat.h"
#include "router.h"
#include "session.h"
#include "protocol.h"
#include <stdio.h>
#include<string.h>

int main(void)
{
    printf("====================================\n");
    printf("       DevHub Router Test\n");
    printf("====================================\n\n");

    /*
        Create a client session.

        Initially the client is NOT authenticated.
    */

    ClientSession session;

    session_init(
        &session,
        INVALID_SOCKET
    );

    session_authenticate(&session,"praxx");

    printf("Session authenticated : %d\n",session.authenticated);
    printf("Username : %s\n\n",session.username);

    // =========================================================
    // CHAT TEST
    // =========================================================

    printf("Testing CHAT routing...\n");
    DevHubHeader chatHeader;
    chatHeader.version=DEVHUB_PROTOCOL_VERSION;
    chatHeader.type=DEVHUB_MSG_CHAT;
    const char* message="Hello from Router_test";

    chatHeader.payloadLength=(unsigned int)strlen(message);

    int result=router_dispatch(&session,&chatHeader,(const unsigned char *)message);
    if(result){
        printf("CHAT routing test PASSED\n");
    }else{
        printf("CHAT routing test FAILED\n");
    }

    printf("\nTesting CHAT response...\n");
    unsigned char responseBuffer[64];

    int responseSize=chat_build_response(responseBuffer,sizeof(responseBuffer));

    if(responseSize>0){
        responseBuffer[responseSize]='\0';
        printf("CHAT response : %s\n",responseBuffer);
        printf("CHAT response test PASSED.\n");
    }else{
        printf("CHAT response test FAILED\n");
    }



    printf("====================================\n");
    printf("       Router Test Complete\n");
    printf("====================================\n");

    printf("\nTesting unauthenticated CHAT...\n");

ClientSession unauthenticatedSession;

session_init(
    &unauthenticatedSession,
    INVALID_SOCKET
);

printf(
    "Unauthenticated session: %d\n",
    unauthenticatedSession.authenticated
);

DevHubHeader unauthChatHeader;

unauthChatHeader.version =
    DEVHUB_PROTOCOL_VERSION;

unauthChatHeader.type =
    DEVHUB_MSG_CHAT;

const char *unauthMessage =
    "This should be rejected";

unauthChatHeader.payloadLength =
    (unsigned int)strlen(unauthMessage);

int unauthResult =
    router_dispatch(
        &unauthenticatedSession,
        &unauthChatHeader,
        (const unsigned char *)unauthMessage
    );

if (!unauthResult)
{
    printf(
        "Unauthenticated CHAT test PASSED.\n"
    );
}
else
{
    printf(
        "Unauthenticated CHAT test FAILED.\n"
    );
}

    return 0;
}