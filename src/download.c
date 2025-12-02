#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "parser.h"
#include "getip.h"
#include "clientTCP.h"
#include "create_file.h"

int main(int argc, char *argv[]){

    if (argc != 2){
        return 1;
    }

    struct URL url;

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

    getip(&url);
    printf("IP Address : %s\n", url.ip);

    // Step 3

    if (term_A1(url)){
        return 1;
    }
    
    // Step 4

    if (term_B1()){
        return 1;
    }

    // Step 5

    if (term_A2(url)){
        return 1;
    }

    // Step 6
    char *content;
    if (term_B2(&content)){
        return 1;
    } 

    // Step 7 

    if (create_file(url, content)){
        return 1;
    }

    free_url(&url);

    return 0;
}