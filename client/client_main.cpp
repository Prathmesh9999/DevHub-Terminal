#include <iostream>

extern "C"
{
    #include "logger.h"
}

int main()
{
    log_info("DevHub Client v1.0 starting");

    std::cout << "DevHub Client v1.0" << std::endl;

    log_info("Client initialization complete");

    return 0;
}