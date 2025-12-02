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

#define SERVER_PORT 21
#define SERVER_ADDR "193.137.29.15"
#define BUF_SIZE 100000

int sockfd;
struct sockaddr_in server_addr;

int configure_socket(struct URL url){
    /*server address handling*/
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(url.ip);    /*32 bit Internet address network byte ordered*/
    server_addr.sin_port = htons(SERVER_PORT);        /*server TCP port must be network byte ordered */
    
    /*open a TCP socket*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return 1;
    }
    
    /*connect to the server*/
    if (connect(sockfd,
                (struct sockaddr *) &server_addr,
                sizeof(server_addr)) < 0) {
        perror("connect()");
        return 1;
    }

    char read_buf[BUF_SIZE];
    read(sockfd, &read_buf, BUF_SIZE);
    printf("%s",read_buf);
    
    return 0;
}

/*int term_B2(struct URL url){
    // To Implement

    return 0;
}

int term_A2(struct URL url){
    // To Implement

    return 0;
}

int term_B1(struct URL url){
    // To Implement

    return 0;
}*/

int term_A1(struct URL url) {
    if (configure_socket(url)) {
        return 1;
    }

    char *buf1 = NULL, *buf2 = NULL;
    asprintf(&buf1, "USER %s\r\n", url.user);
    asprintf(&buf2, "PASS %s\r\n", url.password);
    char buf3[] = "PASV\r\n";
    char response[BUF_SIZE];
    ssize_t bytes;
    int code;

    // Send USER command
    write(sockfd, buf1, strlen(buf1));
    
    // Read and check USER response
    bytes = read(sockfd, response, BUF_SIZE - 1);
    if (bytes <= 0) return 1;
    response[bytes] = '\0';
    printf("%s", response);
    
    sscanf(response, "%d", &code);
    if (code != 331) {
        fprintf(stderr, "USER command failed (expected 331, got %d)\n", code);
        free(buf1);
        free(buf2);
        return 1;
    }

    // Send PASS command
    write(sockfd, buf2, strlen(buf2));
    
    // Read and check PASS response
    bytes = read(sockfd, response, BUF_SIZE - 1);
    if (bytes <= 0) return 1;
    response[bytes] = '\0';
    printf("%s", response);
    
    sscanf(response, "%d", &code);
    if (code != 230) {
        fprintf(stderr, "Login failed (expected 230, got %d)\n", code);
        free(buf1);
        free(buf2);
        return 1;
    }

    // Send PASV command
    write(sockfd, buf3, strlen(buf3));
    
    // Read and check PASV response
    bytes = read(sockfd, response, BUF_SIZE - 1);
    if (bytes <= 0) return 1;
    response[bytes] = '\0';
    printf("%s", response);
    
    sscanf(response, "%d", &code);
    if (code != 227) {
        fprintf(stderr, "PASV command failed (expected 227, got %d)\n", code);
        free(buf1);
        free(buf2);
        return 1;
    }

    // Parse PASV response
    int ip1, ip2, ip3, ip4, port1, port2;
    if (sscanf(response, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)",
               &ip1, &ip2, &ip3, &ip4, &port1, &port2) == 6) {
        printf("Data connection: %d.%d.%d.%d:%d\n", 
               ip1, ip2, ip3, ip4, port1 * 256 + port2);
    }

    // Cleanup
    free(buf1);
    free(buf2);
    close(sockfd);
    
    return 0;
}