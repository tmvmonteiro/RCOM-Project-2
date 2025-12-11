#include "parser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int parse(struct URL *url, char *input) {
    url->protocol = NULL;
    url->user = NULL;
    url->password = NULL;
    url->host = NULL;
    url->path = NULL;
    url->ip = NULL;
    
    if (input == NULL || strlen(input) == 0) {
        fprintf(stderr, "Error: Empty or NULL input string\n");
        return 1;
    }
    
    // Make a copy we can modify
    char *str = strdup(input);
    if (str == NULL) return 1;
    
    // Parse protocol
    char *proto_end = strstr(str, "://");
    if (proto_end == NULL) {
        fprintf(stderr, "Error: Invalid URL format - missing protocol separator\n");
        free(str);
        return 1;
    }
    *proto_end = '\0';
    url->protocol = strdup(str);
    
    // Check protocol
    if (strcmp(url->protocol, "ftp") != 0) {
        fprintf(stderr, "Error: Unsupported protocol\n");
        free(str);
        return 1;
    }
    
    char *ptr = proto_end + 3;
    
    // First, check if there's an '@' to determine if we have credentials
    char *at_ptr = strchr(ptr, '@');
    
    if (at_ptr != NULL) {
        // We have '@', so parse credentials
        *at_ptr = '\0';  // Split at '@'
        
        char *credentials = ptr;
        char *colon_ptr = strchr(credentials, ':');
        
        if (colon_ptr != NULL) {
            // We have "user:password" format
            *colon_ptr = '\0';
            url->user = strdup(credentials);
            url->password = strdup(colon_ptr + 1);
            
            // Check if either user or password is empty
            if ((url->user && strlen(url->user) == 0) || 
                (url->password && strlen(url->password) == 0)) {
                fprintf(stderr, "Error: Both user and password must be specified when using credentials\n");
                free(str);
                return 1;
            }
        } else {
            // We have only user (no password) - ERROR
            fprintf(stderr, "Error: Password missing - both user and password must be specified\n");
            free(str);
            return 1;
        }
        
        ptr = at_ptr + 1;  // Move past '@'
    } else {
        // No '@' found, check if there's a colon that might be confused as credentials
        char *colon_in_path = strchr(ptr, ':');
        if (colon_in_path != NULL) {
            // There's a colon but no '@' - this is an error
            // Example: "ftp://anonymous:anonymousftp.bit.nl/speedtest/100mb.bin"
            fprintf(stderr, "Error: Invalid URL format - missing '@' separator\n");
            free(str);
            return 1;
        }
        
        // No credentials and no confusing colon - use anonymous
        url->user = strdup("anonymous");
        url->password = strdup("anonymous");
    }
    
    // Parse host and path
    char *slash_ptr = strchr(ptr, '/');
    if (slash_ptr != NULL) {
        *slash_ptr = '\0';
        url->host = strdup(ptr);
        url->path = strdup(slash_ptr + 1);
    } else {
        url->host = strdup(ptr);
        url->path = strdup("/");
    }
    
    // Validate host
    if (url->host == NULL || strlen(url->host) == 0) {
        fprintf(stderr, "Error: No host specified\n");
        free(str);
        return 1;
    }
    
    // Validate path is not empty (except for root)
    if (url->path && strlen(url->path) == 0) {
        free(url->path);
        url->path = strdup("/");
    }
    
    free(str);
    return 0;
}

void free_url(struct URL *url) {
    if (url->protocol) free(url->protocol);
    if (url->user) free(url->user);
    if (url->password) free(url->password);
    if (url->host) free(url->host);
    if (url->path) free(url->path);
    if (url->ip) free(url->ip);
}