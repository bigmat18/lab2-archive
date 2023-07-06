#include <pthread.h>
#include <stdio.h>
#include "hash_table.h"

// Macro creata per controllare gli errori nei valori di ritorno
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

/**
 * @details Funzione che alloca una sruttura buffer usata
 * @return Ritorna un puntatore alla nuova struttura allocata
*/
buffer_t *buffer_create();

/**
 * @details Funzione inserisce nel buffer una stringa usando i mutex e le condvar
 * 
 * @param *buffer: buffer in cui aggiungere elemento
 * @param *str: stringa da aggiungere
 */
void buffer_insert(buffer_t *buffer, char *str);

/**
 * @details Funzione che rimuove dal buffer un elemento basandosi su cindex all'interno di buffer
 * @param *buffer: buffer su cui rimuovere elemento
 * @return ritorna il valore rimosso
*/
char *buffer_remove(buffer_t *buffer);

/**
 * @details Funzione che ripulische la memoria ed elimina tutto quello che è legato alla struttura buffer
 * @param *buffer: buffer da pulire
 */
void buffer_destroy(buffer_t *buffer);

#endif
