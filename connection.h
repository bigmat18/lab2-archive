#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// Macro creata per controllare gli errori nei valori di ritorno
#define check(val, str, result)                                          \
    if (val)                                                             \
    {                                                                    \
        if (errno == 0)                                                  \
            fprintf(stderr, "== %s\n", str);                             \
        else                                                             \
            fprintf(stderr, "== %s: %s\n", str, strerror(errno));        \
        fprintf(stderr, "== Linea: %d, File: %s\n", __LINE__, __FILE__); \
        result;                                                          \
    }

#define HOST "127.0.0.1"
#define PORT 58449

#ifndef CONNECTION_H
#define CONNECTION_H

typedef struct {
    int fd_skt;
    struct sockaddr_in serv_addr;
} connection_t;

connection_t *connection_create();

void connection_destroy(connection_t *connection);

#endif