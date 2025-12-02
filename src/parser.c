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
    
    // Check for empty input
    if (input == NULL || strlen(input) == 0) {
        fprintf(stderr, "Error: Empty or NULL input string\n");
        return 1;
    }
    
    char *ptr = input;
    char *colon_ptr, *slash_ptr, *at_ptr;
    
    // Parse protocol
    colon_ptr = strstr(ptr, "://");
    if (colon_ptr == NULL) {
        fprintf(stderr, "Error: Invalid URL format - missing '://' protocol separator\n");
        return 1;
    }
    
    // Extract protocol
    int protocol_len = colon_ptr - ptr;
    if (protocol_len <= 0) {
        fprintf(stderr, "Error: Protocol name is empty\n");
        return 1;
    }
    
    url->protocol = (char *)malloc(protocol_len + 1);
    if (url->protocol == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for protocol\n");
        return 1;
    }
    strncpy(url->protocol, ptr, protocol_len);
    url->protocol[protocol_len] = '\0';
    
    // Verify protocol
    if (strcmp(url->protocol, "ftp") != 0) {
        fprintf(stderr, "Error: Unsupported protocol '%s' - only 'ftp' is supported\n", url->protocol);
        return 1; 
    }
    
    ptr = colon_ptr + 3;
    
    // Check for user:password@
    at_ptr = strchr(ptr, '@');
    
    if (at_ptr != NULL) {
        // We have user:password@
        char *user_password_part = ptr;
        char *user_pass_sep = strchr(user_password_part, ':');
        
        if (user_pass_sep != NULL && user_pass_sep < at_ptr) {
            // Extract user
            int user_len = user_pass_sep - user_password_part;
            if (user_len > 0) {
                url->user = (char *)malloc(user_len + 1);
                if (url->user == NULL) {
                    fprintf(stderr, "Error: Memory allocation failed for user\n");
                    return 1;
                }
                strncpy(url->user, user_password_part, user_len);
                url->user[user_len] = '\0';
            } else {
                url->user = NULL;
            }
            
            // Extract password
            int password_len = at_ptr - (user_pass_sep + 1);
            if (password_len > 0) {
                url->password = (char *)malloc(password_len + 1);
                if (url->password == NULL) {
                    fprintf(stderr, "Error: Memory allocation failed for password\n");
                    return 1;
                }
                strncpy(url->password, user_pass_sep + 1, password_len);
                url->password[password_len] = '\0';
            } else {
                url->password = NULL;
            }
        } else {
            // Only user@
            int user_len = at_ptr - ptr;
            if (user_len > 0) {
                url->user = (char *)malloc(user_len + 1);
                if (url->user == NULL) {
                    fprintf(stderr, "Error: Memory allocation failed for user\n");
                    return 1;
                }
                strncpy(url->user, ptr, user_len);
                url->user[user_len] = '\0';
            }
            url->password = NULL;
        }
        
        ptr = at_ptr + 1;
    } else {
        url->user = NULL;
        url->password = NULL;
    }
    
    // Find the host
    slash_ptr = strchr(ptr, '/');
    if (slash_ptr == NULL) {
        // No path specified
        int host_len = strlen(ptr);
        if (host_len > 0) {
            url->host = (char *)malloc(host_len + 1);
            if (url->host == NULL) {
                fprintf(stderr, "Error: Memory allocation failed for host\n");
                return 1;
            }
            strcpy(url->host, ptr);
            url->host[host_len] = '\0';
        } else {
            fprintf(stderr, "Error: No host specified in URL\n");
            return 1;
        }
        
        // Empty path
        url->path = (char *)malloc(1);
        if (url->path == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for path\n");
            return 1;
        }
        url->path[0] = '\0';
        
        return 0;
    }
    
    // Extract host
    int host_len = slash_ptr - ptr;
    if (host_len <= 0) {
        fprintf(stderr, "Error: No host specified in URL\n");
        return 1;
    }
    
    url->host = (char *)malloc(host_len + 1);
    if (url->host == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for host\n");
        return 1;
    }
    strncpy(url->host, ptr, host_len);
    url->host[host_len] = '\0';
    
    // Move to path
    ptr = slash_ptr;
    
    // Extract path
    if (strlen(ptr) > 0) {
        url->path = (char *)malloc(strlen(ptr) + 1);
        if (url->path == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for path\n");
            return 1;
        }
        strcpy(url->path, ptr);
    } else {
        url->path = (char *)malloc(2);
        if (url->path == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for path\n");
            return 1;
        }
        url->path[0] = '/';
        url->path[1] = '\0';
    }
    
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