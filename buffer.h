#include <pthread.h>
#include <stdio.h>

#ifndef check(val, str, result)
    #define check(val, str, result) \
        if (val)                    \
            perror(str);            \
        result;
#endif

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

int buffer_insert(buffer_t *buffer, const char *str);

char* buffer_remove(buffer_t *buffer);

void buffer_destroy(buffer_t *buffer);

#endif


