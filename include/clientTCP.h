#ifndef CLIENTTCP_H
#define CLIENTTCP_H

#include <netinet/in.h>
#include "parser.h"

int read_line(int sockfd, char *buf, size_t maxlen);
int read_ftp_response(int sockfd, int *code);
int configure_socket(char *ip, uint16_t port,int *sockfd);
int term_B2(char **content);
int term_A2(struct URL url);
int term_B1();
int term_A1(struct URL url);

#endif /* CLIENTTCP_H */