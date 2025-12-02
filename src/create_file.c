#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

int create_file(struct URL url, char *content) {
     // Check for valid inputs
    if (content == NULL) {
        fprintf(stderr, "Error: Content is NULL\n");
        return 1;
    }

    char *filename = url.path;
    char *last_slash = strrchr(url.path, '/');
    
    if (last_slash != NULL) {
        filename = last_slash + 1;
        if (*filename == '\0') {
            filename = url.path;
        }
    }

    char filepath[512];
    snprintf(filepath, sizeof(filepath), "download/%s", filename);
    
    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot create file '%s'\n", filepath);
        return 1;
    }

    // Write content to file
    size_t content_len = strlen(content);
    size_t bytes_written = fwrite(content, 1, content_len, file);

    if (bytes_written != content_len) {
        fprintf(stderr, "Error: Failed to write all content. Written %zu of %zu bytes\n", 
                bytes_written, content_len);
        fclose(file);
        return 1;
    }

    // Close file
    if (fclose(file) != 0) {
        fprintf(stderr, "Error: Failed to close file '%s'\n", filepath);
        return 1;
    }

    return 0;
}