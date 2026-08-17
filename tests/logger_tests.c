#include<stdio.h>

#include"../logger/logger.h"

int main(void)
{
    log_info("Logger test started");
    log_warning("This is a warning");
    log_error("This is an error");

    return 0;
}