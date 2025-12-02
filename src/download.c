#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "parser.h"
#include "getip.h"

int main(int argc, char *argv[]){

    if (argc != 2){
        return 1;
    }

    struct URL url;
    char *ip = NULL;

    // Step 1 

    if (parse(&url, argv[1])){
        return 1;
    }

    printf("URL Components:\n");
    printf("Protocol: %s\n", url.protocol ? url.protocol : "(not specified)");
    printf("User: %s\n", url.user ? url.user : "(anonymous)");
    printf("Password: %s\n", url.password ? url.password : "(none)");
    printf("Host: %s\n", url.host ? url.host : "(not specified)");
    printf("Path: %s\n", url.path ? url.path : "(root directory)");

    // Step 2 

    getip(&ip, url.host);
    printf("IP Address : %s\n", ip);

    // Step 3
    
    // Step 4

    // Step 5
   
    free_url(&url);

    return 0;
}