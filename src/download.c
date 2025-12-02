#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "parser.h"

int main(int argc, char *argv[]){

    if (argc != 2){
        return 1;
    }

    struct URL url;

    if (parse(&url, argv[1])){
        return 1;
    }

    printf("URL Components:\n");
    printf("Protocol: %s\n", url.protocol ? url.protocol : "(not specified)");
    printf("User: %s\n", url.user ? url.user : "(anonymous)");
    printf("Password: %s\n", url.password ? url.password : "(none)");
    printf("Host: %s\n", url.host ? url.host : "(not specified)");
    printf("Path: %s\n", url.path ? url.path : "(root directory)");
    free_url(&url);

    return 0;
}