#include<iostream>

extern "C"{
    #include"logger.h"
}

int main(){
     log_info("DevHub Server v1.0 starting");

    std::cout << "DevHub Server v1.0" << std::endl;

    log_info("Server initialization complete");
    return 0;
}