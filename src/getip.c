#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "getip.h"
#include "parser.h"

int getip(struct URL *url) {
    if (url->host == NULL || url->host[0] == '\0') {
        fprintf(stderr, "Error: Invalid hostname\n");
        return 1;
    }
    
    struct hostent *h;
    
    if ((h = gethostbyname(url->host)) == NULL) {
        herror("gethostbyname()");
        return 1;
    }
    char *temp_ip = inet_ntoa(*((struct in_addr *) h->h_addr));
    free(url->ip);
    url->ip = strdup(temp_ip);
    if (!url->ip) return 1;

    return 0;
}
