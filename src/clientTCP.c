/**      (C)2000-2021 FEUP
 *       tidy up some includes and parameters
 * */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"

#define CONTROLLER_PORT 21
#define BUF_SIZE 100000

int sockfd1, sockfd2;
struct sockaddr_in server_addr;
int ip1, ip2, ip3, ip4, port1, port2;
ssize_t bytes;
char line[BUF_SIZE];
int code;
FILE *sock_file1, *sock_file2;

int read_ftp_response(FILE *sock_file, int *code) {
    
    while (fgets(line, sizeof(line), sock_file) != NULL) {
        // Remove trailing \r\n
        line[strcspn(line, "\r\n")] = '\0';
        //printf("%s\n", line);  // for debugging
        
        // Parse code from this line
        if (sscanf(line, "%d", code) != 1) {
            return 1;
        }
        
        // Check if this is a termination line
        if (line[3] == ' ') {
            //printf("%d\n", *code); // for debugging
            return 0; 
        }
    }
    
    return 1; 
}

int configure_socket(char *ip, uint16_t port, int *sockfd, FILE **sock_file){
    /*server address handling*/
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);    /*32 bit Internet address network byte ordered*/
    server_addr.sin_port = htons(port);        /*server TCP port must be network byte ordered */
    
    /*open a TCP socket*/
    if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return 1;
    }
    
    /*connect to the server*/
    if (connect(*sockfd,
                (struct sockaddr *) &server_addr,
                sizeof(server_addr)) < 0) {
        perror("connect()");
        return 1;
    }

    /* Create FILE* stream*/
    if (sock_file != NULL) {
        *sock_file = fdopen(*sockfd, "r");
        if (*sock_file == NULL) {
            perror("fdopen failed");
            close(*sockfd);
            return 1;
        }
    }

    if (port == CONTROLLER_PORT){
        char read_buf[BUF_SIZE];
        bytes = read(*sockfd, &read_buf, BUF_SIZE);
    }
    
    return 0;
}

int term_B2(char **content) {
    if (!content || !sock_file2) return 1;
    
    char buffer[4096];
    size_t total = 0;
    *content = malloc(1);
    if (!*content) return 1;
    (*content)[0] = '\0';
    
    while (!feof(sock_file2)) {
        size_t bytes = fread(buffer, 1, sizeof(buffer), sock_file2);
        if (bytes > 0) {
            *content = realloc(*content, total + bytes + 1);
            memcpy(*content + total, buffer, bytes);
            total += bytes;
            (*content)[total] = '\0';
        }
    }
    
    return 0;
}

int term_A2(struct URL url){
    char *buf = NULL;
    bytes = asprintf(&buf, "RETR %s\r\n", url.path);

    // Send RETR command
    bytes = write(sockfd1, buf, strlen(buf));
    if (bytes <= 0) return 1;

    // Read and check RETR response
    if(read_ftp_response(sock_file1, &code)){
        // Insert error print here
        return 1;
    }
    
    if (code != 150 && code != 125) {
        fprintf(stderr, "RETR command failed (expected 150 or 125, got %d)\n", code);
        free(buf);
        return 1;
    }

    // Cleanup
    free(buf);
    close(sockfd1);
    return 0;
}

int term_B1(){
    uint16_t data_port = (256 * port1) + port2;
    char *ip = NULL;
    bytes = asprintf(&ip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);

    if (configure_socket(ip, data_port, &sockfd2, &sock_file2)) {
        return 1;
    }

    // Cleanup
    free(ip);
    return 0;
}

int term_A1(struct URL url) {
    if (configure_socket(url.ip, CONTROLLER_PORT, &sockfd1, &sock_file1)) {
        return 1;
    }

    char *buf1 = NULL, *buf2 = NULL;
    bytes = asprintf(&buf1, "USER %s\r\n", url.user);
    bytes = asprintf(&buf2, "PASS %s\r\n", url.password);
    char buf3[] = "PASV\r\n";

    // Send USER command
    bytes = write(sockfd1, buf1, strlen(buf1));
    if (bytes <= 0) return 1;
    
    // Read and check USER response
    if(read_ftp_response(sock_file1, &code)){
        // Insert error print here
        return 1;
    }
    
    if (code != 331) {
        fprintf(stderr, "USER command failed (expected 331, got %d)\n", code);
        free(buf1);
        free(buf2);
        return 1;
    }

    // Send PASS command
    bytes = write(sockfd1, buf2, strlen(buf2));
    if (bytes <= 0) return 1;

    // Read and check PASS response
    if(read_ftp_response(sock_file1, &code)){
        // Insert error print here
        return 1;
    }
    
    if (code != 230) {
        fprintf(stderr, "Login failed (expected 230, got %d)\n", code);
        free(buf1);
        free(buf2);
        return 1;
    }

    // Send PASV command
    bytes = write(sockfd1, buf3, strlen(buf3));
    if (bytes <= 0) return 1;
    
    // Read and check PASV response
    if(read_ftp_response(sock_file1, &code)){
        // Insert error print here
        return 1;
    }
    
    if (code != 227) {
        fprintf(stderr, "PASV command failed (expected 227, got %d)\n", code);
        free(buf1);
        free(buf2);
        return 1;
    }

    // Parse PASV response
    if (sscanf(line, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)",
               &ip1, &ip2, &ip3, &ip4, &port1, &port2) == 6) {
        printf("Data connection: %d.%d.%d.%d:%d\n", 
               ip1, ip2, ip3, ip4, port1 * 256 + port2);
    }

    // Cleanup
    free(buf1);
    free(buf2);
    
    return 0;
}