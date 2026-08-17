#include <iostream>

extern "C"
{
    #include "logger.h"
}

#include "Config.hpp"

int main()
{
    log_info("DevHub Client v1.0 starting");

    Config config;

    if (!config.load("config/config.ini"))
    {
        log_error("Failed to load configuration");
        return 1;
    }

    std::string serverIP = config.get("server", "ip");
    int serverPort = config.getInt("server", "port");

    std::cout << "Server IP: " << serverIP << std::endl;
    std::cout << "Server Port: " << serverPort << std::endl;

    log_info("Configuration loaded successfully");

    return 0;
}