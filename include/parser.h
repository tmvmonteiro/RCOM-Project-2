#ifndef PARSER_H
#define PARSER_H

struct URL
{
    char *protocol;
    char *user;
    char *password;
    char *host;
    char *path;
    char *ip;
};

int parse(struct URL *url, char *input);
void free_url(struct URL *url);

#endif /* PARSER_H */