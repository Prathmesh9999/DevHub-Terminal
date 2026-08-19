#include<logger.h>

#include<stdio.h>

void log_info(const char *message){
    printf("[INFO]%s\n",message);
}
void log_warning(const char *message){
    printf("[WARNING]%s\n",message);
}
void log_error(const char *message){
    printf("[ERROR]%s\n",message);
}