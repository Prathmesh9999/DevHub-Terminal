#include "../core/auth.h"
#include "router.h"
#include <stdio.h>
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
        (SOCKET)1234
    );

    printf("Initial session state:\n");
    printf(
        "Authenticated: %s\n\n",
        session_is_authenticated(&session)
            ? "YES"
            : "NO"
    );


    /*
        ------------------------------------------------
        TEST 1: CHAT WITHOUT AUTHENTICATION
        ------------------------------------------------

        This should be rejected by the router.
    */

    printf("TEST 1: CHAT without authentication\n");

    DevHubHeader header;

    header.version =
        DEVHUB_PROTOCOL_VERSION;

    header.type =
        DEVHUB_MSG_CHAT;

    const unsigned char chatPayload[] =
        "Hello";

    header.payloadLength =
        sizeof(chatPayload) - 1;

    int result = router_dispatch(
        &session,
        &header,
        chatPayload
    );

    printf(
        "Result: %s\n\n",
        result
            ? "UNEXPECTED SUCCESS"
            : "CORRECTLY REJECTED"
    );


    /*
        ------------------------------------------------
        TEST 2: AUTHENTICATE SESSION
        ------------------------------------------------
    */

    printf("TEST 2: Authenticate session\n");

    result = session_authenticate(
        &session,
        "admin"
    );

    printf(
        "Authentication: %s\n",
        result
            ? "SUCCESS"
            : "FAILED"
    );

    printf(
        "Authenticated: %s\n",
        session_is_authenticated(&session)
            ? "YES"
            : "NO"
    );

    printf(
        "Username: %s\n\n",
        session.username
    );


    /*
        ------------------------------------------------
        TEST 3: CHAT AFTER AUTHENTICATION
        ------------------------------------------------

        This should now reach chat_handle().
    */

    printf("TEST 3: CHAT after authentication\n");

    header.type =
        DEVHUB_MSG_CHAT;

    result = router_dispatch(
        &session,
        &header,
        chatPayload
    );

    printf(
        "Result: %s\n\n",
        result
            ? "SUCCESS"
            : "FAILED"
    );


    /*
        ------------------------------------------------
        TEST 4: AUTH MESSAGE
        ------------------------------------------------

        AUTH is allowed even when the session is not
        authenticated because authentication must happen
        before protected operations.
    */

    printf("TEST 4: AUTH message\n");

    unsigned char authPayload[256];

    int authPayloadSize =
        auth_build_payload(
            "praxx",
            "devhub999",
            authPayload,
            sizeof(authPayload)
        );

    if (authPayloadSize < 0)
    {
        printf(
            "Failed to build AUTH payload.\n"
        );

        return 1;
    }

    header.type =
        DEVHUB_MSG_AUTH;

    header.payloadLength =
        (unsigned int)authPayloadSize;

    result = router_dispatch(
        &session,
        &header,
        authPayload
    );

    printf(
        "AUTH result: %s\n\n",
        result
            ? "SUCCESS"
            : "FAILED"
    );


    /*
        ------------------------------------------------
        TEST 5: FILE UPLOAD
        ------------------------------------------------

        The router should allow the message because
        the session is authenticated.
    */

    printf("TEST 5: FILE_UPLOAD after authentication\n");

    header.type =
        DEVHUB_MSG_FILE_UPLOAD;

    header.payloadLength = 0;

    result = router_dispatch(
        &session,
        &header,
        NULL
    );

    printf(
        "Result: %s\n\n",
        result
            ? "SUCCESS"
            : "FAILED"
    );


    /*
        ------------------------------------------------
        TEST 6: FILE DOWNLOAD
        ------------------------------------------------
    */

    printf("TEST 6: FILE_DOWNLOAD after authentication\n");

    header.type =
        DEVHUB_MSG_FILE_DOWNLOAD;

    header.payloadLength = 0;

    result = router_dispatch(
        &session,
        &header,
        NULL
    );

    printf(
        "Result: %s\n\n",
        result
            ? "SUCCESS"
            : "FAILED"
    );


    /*
        ------------------------------------------------
        TEST 7: COMMAND
        ------------------------------------------------
    */

    printf("TEST 7: COMMAND after authentication\n");

    header.type =
        DEVHUB_MSG_COMMAND;

    header.payloadLength = 0;

    result = router_dispatch(
        &session,
        &header,
        NULL
    );

    printf(
        "Result: %s\n\n",
        result
            ? "SUCCESS"
            : "FAILED"
    );


    /*
        ------------------------------------------------
        TEST 8: LOGOUT
        ------------------------------------------------

        After logout, protected messages should again
        be rejected.
    */

    printf("TEST 8: Logout session\n");

    session_logout(&session);

    printf(
        "Authenticated: %s\n\n",
        session_is_authenticated(&session)
            ? "YES"
            : "NO"
    );


    /*
        ------------------------------------------------
        TEST 9: CHAT AFTER LOGOUT
        ------------------------------------------------

        This should be rejected.
    */

    printf("TEST 9: CHAT after logout\n");

    header.type =
        DEVHUB_MSG_CHAT;

    header.payloadLength =
        sizeof(chatPayload) - 1;

    result = router_dispatch(
        &session,
        &header,
        chatPayload
    );

    printf(
        "Result: %s\n\n",
        result
            ? "UNEXPECTED SUCCESS"
            : "CORRECTLY REJECTED"
    );


    /*
        ------------------------------------------------
        TEST 10: INVALID MESSAGE TYPE
        ------------------------------------------------
    */

    printf("TEST 10: Invalid message type\n");

    header.type = 99;

    header.payloadLength = 0;

    result = router_dispatch(
        &session,
        &header,
        NULL
    );

    printf(
        "Result: %s\n\n",
        result
            ? "UNEXPECTED SUCCESS"
            : "CORRECTLY REJECTED"
    );


    printf("====================================\n");
    printf("       Router Test Complete\n");
    printf("====================================\n");

    return 0;
}