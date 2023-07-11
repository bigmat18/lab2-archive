#define _GNU_SOURCE
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
    pthread_mutex_t file_mutex;
    pthread_mutex_t *interrupt_mutex;
} data_t;

typedef struct {
    hash_table_t *hash_table;
    pthread_mutex_t *interrupt_mutex;
} handler_data_t;

/**
 * @details Funzione che alloca una struttura thread_t.
 * 
 * @param *data: dati usati dal thread come paramentro in tbody
 * @param *(*tbody): puntatore alla funzione da far partire con il thread
 * 
 * @return Ritorna puntatore alla stuttura allocata.
*/
thread_t *thread_create(void *data, void *(*tbody)(void *));

/**
 * @details Funzione che dealloca un thread con tutti i dati ad esso legato.
 * @param *thread: thread da deallocare
*/
void thread_destroy(thread_t *thread);

#endif

