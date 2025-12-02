#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "getip.h"

int getip(char **ip, char *host) {
    if (host == NULL || host[0] == '\0') {
        fprintf(stderr, "Error: Invalid hostname\n");
        return 1;
    }
    
    struct hostent *h;
    
    if ((h = gethostbyname(host)) == NULL) {
        herror("gethostbyname()");
        return 1;
    }
    *ip = inet_ntoa(*((struct in_addr *) h->h_addr));

    return 0;
}
