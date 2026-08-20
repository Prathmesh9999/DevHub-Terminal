
#include "router.h"
#include <stdio.h>


int main()
{
    DevHubHeader header;

    header.version = DEVHUB_PROTOCOL_VERSION;
    header.type = DEVHUB_MSG_CHAT;
    header.payloadLength = 5;

    const unsigned char payload[] = "Hello";

    printf("Testing router...\n");

    int result = router_dispatch(&header, payload);

    printf("Router result: %s\n",result ? "SUCCESS" : "FAILED");

    printf("\nTesting AUTH routing...\n");

header.type = DEVHUB_MSG_AUTH;

result = router_dispatch(
    &header,
    payload
);

printf(
    "AUTH route result: %s\n",
    result ? "SUCCESS" : "FAILED"
);

printf("\nTesting invalid routing...\n");

header.type = 99;

result = router_dispatch(
    &header,
    payload
);

printf(
    "Invalid route result: %s\n",
    result ? "UNEXPECTED SUCCESS" : "CORRECTLY REJECTED"
);

    return 0;
}
