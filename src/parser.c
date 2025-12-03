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
        fprintf(stderr, "Error: Invalid URL format\n");
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
    
    // Get user/password 
    char *at_ptr = strchr(ptr, '@');
    if (at_ptr != NULL) {
        *at_ptr = '\0';  // Split at '@'
        
        char *colon_ptr = strchr(ptr, ':');
        if (colon_ptr != NULL) {
            *colon_ptr = '\0';
            url->user = strdup(ptr);
            url->password = strdup(colon_ptr + 1);
        } else {
            url->user = strdup(ptr);
            url->password = strdup("anonymous");
        }
        
        ptr = at_ptr + 1;  // Move past '@'
    } 

    else {
        url->user = strdup("anonymous");
        url->password = strdup("anonymous");
    }
    
    // Handle empty user
    if (url->user && strlen(url->user) == 0) {
        free(url->user);
        url->user = strdup("anonymous");
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