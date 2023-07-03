#include <pthread.h>
#include <stdio.h>
#include "hash_table.h"

#define check(val, str, result)                                                          \
    if (val) {                                                                           \
        fprintf(stderr, "== %s == Linea: %d, File: %s\n", str, __LINE__, __FILE__);      \
        result;                                                                          \
    }

#define PC_BUFFER_LEN 10

#ifndef BUFFER_H
#define BUFFER_H

typedef struct {
    char **buffer;

    pthread_cond_t empty;
    pthread_cond_t full;
    pthread_mutex_t mutex;

    unsigned int index, pindex, cindex;
} buffer_t;


buffer_t *buffer_create();

int buffer_insert(buffer_t *buffer, char *str);

char *buffer_remove(buffer_t *buffer);

void buffer_destroy(buffer_t *buffer);

#endif
