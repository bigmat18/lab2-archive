#include "buffer.h"
#include "hash_table.h"

#ifndef THREAD_H
#define THREAD_H

typedef struct {
    pthread_t thread;
    void *data;
    void *(*tbody)(void *);
} thread_t;

typedef struct {
    buffer_t *buffer;
    hash_table_t *hash_table;
    int pipe;
    FILE *file;
} data_t;

thread_t *thread_create(void *data, void *(*tbody)(void *));

void thread_destroy(thread_t *thread);

#endif

