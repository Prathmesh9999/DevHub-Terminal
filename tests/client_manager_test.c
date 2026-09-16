#include "../core/client_manager.h"

#include <stdio.h>

int main(void)
{
    client_manager_init();

    SOCKET fakeSocket = (SOCKET)100;

    int index = client_manager_add(fakeSocket);

    if (index == -1)
    {
        printf("Failed to add client.\n");
        return 1;
    }

    printf("Client added at slot: %d\n", index);

    ClientConnection *client = client_manager_get(index);

    if (client == NULL)
    {
        printf("Failed to get client.\n");
        return 1;
    }

    printf("Client socket: %llu\n",
           (unsigned long long)client->session.socket);

    printf("Client active: %d\n", client->active);

    client_manager_remove(index);

    if (client_manager_get(index) == NULL)
    {
        printf("Client removed successfully.\n");
    }

    return 0;
}