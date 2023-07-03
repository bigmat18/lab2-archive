#include <stdlib.h>
#include <stdio.h>
#include "buffer.h"

buffer_t *buffer_create() {
    buffer_t *buffer_strc = (buffer_t*)malloc(sizeof(buffer_t));
    check(buffer_strc == NULL, "Errore allocazione buffer", exit(1));

    buffer_strc->buffer = (char **)malloc(sizeof(char *) * PC_BUFFER_LEN);
    check(buffer_strc->buffer == NULL, "Errore allocazione buffer", exit(1));

    buffer_strc->cindex = 0;
    buffer_strc->index = 0;
    buffer_strc->pindex = 0;

    if (pthread_mutex_init(&buffer_strc->mutex, NULL) != 0)
        goto mutex_cleanup;

    if (pthread_cond_init(&buffer_strc->empty, NULL) != 0)
        goto empty_cleanup;
    
    if (pthread_cond_init(&buffer_strc->full, NULL) != 0) 
        goto full_cleanup;

    return buffer_strc;

    full_cleanup:
        check(pthread_cond_destroy(&buffer_strc->empty) != 0, "Errore destroy", exit(1));

    empty_cleanup:
        check(pthread_mutex_destroy(&buffer_strc->mutex) != 0, "Errore destroy", exit(1));

    mutex_cleanup:
        free(buffer_strc->buffer);
        free(buffer_strc);

    perror("Errore nella creazione delle variabili per la gestione del buffer");
    return NULL;
}

int buffer_insert(buffer_t *buffer, char *str) {
    check(pthread_mutex_lock(&buffer->mutex) != 0, "Errore lock insert buffer", pthread_exit(NULL));

    while (buffer->index == PC_BUFFER_LEN)
        check(pthread_cond_wait(&buffer->full, &buffer->mutex) != 0, "Errore wait insert buffer", pthread_exit(NULL));

    buffer->buffer[buffer->pindex % PC_BUFFER_LEN] = str;
    buffer->index += 1;
    buffer->pindex += 1;

    check(pthread_cond_signal(&buffer->empty) != 0, "Errore signal insert buffer", pthread_exit(NULL));
    check(pthread_mutex_unlock(&buffer->mutex) != 0, "Errore unlock insert buffer", pthread_exit(NULL));
}

char *buffer_remove(buffer_t *buffer, FILE *file) {
    char *result = NULL;

    check(pthread_mutex_lock(&buffer->mutex) != 0, "Errore lock remove buffer", pthread_exit(NULL));
    
    while (buffer->index == 0)
        check(pthread_cond_wait(&buffer->empty, &buffer->mutex) != 0, "Errore wait remove buffer", pthread_exit(NULL));

    result = buffer->buffer[buffer->cindex % PC_BUFFER_LEN];

    if (file != NULL) {
        FILE *file2= fopen("lettori2.log", "a");
        int val = hash_table_count(result);
        fprintf(stderr, "%s %d\n", result, val);
        fprintf(file2, "%s %d\n", result, val);
        fprintf(file, "%s %d\n", result, val);
        fclose(file2);
    }

    buffer->cindex += 1;
    buffer->index -= 1;

    check(pthread_cond_signal(&buffer->full) != 0, "Errore signal remove buffer", pthread_exit(NULL));
    check(pthread_mutex_unlock(&buffer->mutex) != 0, "Errore unlock remove buffer", pthread_exit(NULL));

    return result;
}

void buffer_destroy(buffer_t *buffer) {
    for(unsigned int i = 0; i < PC_BUFFER_LEN; i++)
        free(buffer->buffer[i]);

    free(buffer->buffer);
    check(pthread_mutex_destroy(&buffer->mutex) != 0, "Errore destroy", exit(1));
    check(pthread_cond_destroy(&buffer->empty) != 0, "Errore destroy", exit(1));
    check(pthread_cond_destroy(&buffer->full) != 0, "Errore destroy", exit(1));

    free(buffer);
}