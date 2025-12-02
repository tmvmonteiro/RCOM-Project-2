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

#include "parser.h"

#define CONTROLLER_PORT 21
#define BUF_SIZE 100000

int sockfd1, sockfd2;
struct sockaddr_in server_addr;
int ip1, ip2, ip3, ip4, port1, port2;
ssize_t bytes;
char response[BUF_SIZE];
int code;

int configure_socket(char *ip, uint16_t port, int *sockfd){
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

    if (port == CONTROLLER_PORT){
        char read_buf[BUF_SIZE];
        bytes = read(*sockfd, &read_buf, BUF_SIZE);
    }
    
    return 0;
}

int term_B2(char **content){
    bytes = read(sockfd2, response, BUF_SIZE - 1);
    if (bytes <= 0) return 1;
    response[bytes] = '\0';
    *content = response;

    close(sockfd1);
    close(sockfd2);
    return 0;
}

int term_A2(struct URL url){
    char *buf = NULL;
    bytes = asprintf(&buf, "RETR %s\r\n", url.path);

    // Send RETR command
    bytes = write(sockfd1, buf, strlen(buf));

    // Read and check RETR response
    bytes = read(sockfd1, response, BUF_SIZE - 1);
    if (bytes <= 0) return 1;
    response[bytes] = '\0';
    
    
    sscanf(response, "%d", &code);
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

    if (configure_socket(ip, data_port, &sockfd2)) {
        return 1;
    }

    // Cleanup
    free(ip);
    return 0;
}

int term_A1(struct URL url) {
    if (configure_socket(url.ip, CONTROLLER_PORT, &sockfd1)) {
        return 1;
    }

    char *buf1 = NULL, *buf2 = NULL;
    bytes = asprintf(&buf1, "USER %s\r\n", url.user);
    bytes = asprintf(&buf2, "PASS %s\r\n", url.password);
    char buf3[] = "PASV\r\n";

    // Send USER command
    bytes = write(sockfd1, buf1, strlen(buf1));
    
    // Read and check USER response
    bytes = read(sockfd1, response, BUF_SIZE - 1);
    if (bytes <= 0) return 1;
    response[bytes] = '\0';
    
    
    sscanf(response, "%d", &code);
    if (code != 331) {
        fprintf(stderr, "USER command failed (expected 331, got %d)\n", code);
        free(buf1);
        free(buf2);
        return 1;
    }

    // Send PASS command
    bytes = write(sockfd1, buf2, strlen(buf2));
    
    // Read and check PASS response
    bytes = read(sockfd1, response, BUF_SIZE - 1);
    if (bytes <= 0) return 1;
    response[bytes] = '\0';
    
    
    sscanf(response, "%d", &code);
    if (code != 230) {
        fprintf(stderr, "Login failed (expected 230, got %d)\n", code);
        free(buf1);
        free(buf2);
        return 1;
    }

    // Send PASV command
    bytes = write(sockfd1, buf3, strlen(buf3));
    
    // Read and check PASV response
    bytes = read(sockfd1, response, BUF_SIZE - 1);
    if (bytes <= 0) return 1;
    response[bytes] = '\0';
    
    
    sscanf(response, "%d", &code);
    if (code != 227) {
        fprintf(stderr, "PASV command failed (expected 227, got %d)\n", code);
        free(buf1);
        free(buf2);
        return 1;
    }

    // Parse PASV response
    if (sscanf(response, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)",
               &ip1, &ip2, &ip3, &ip4, &port1, &port2) == 6) {
        printf("Data connection: %d.%d.%d.%d:%d\n", 
               ip1, ip2, ip3, ip4, port1 * 256 + port2);
    }

    // Cleanup
    free(buf1);
    free(buf2);
    
    return 0;
}