#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"

hash_table_t *hash_table_create() {
    // Creazione hash table
    int id = hcreate(NUM_ELEM);
    check(id == 0, "Errore nella creazione dell'hash table", exit(1))

    // Creazione struttura che gestisce l'hash table
    hash_table_t *hash_table = (hash_table_t*)malloc(sizeof(hash_table_t));

    if (hash_table == NULL)
        goto hash_table_cleanup;

    hash_table->hash_table_id = id;
    hash_table->size_entrys = 100;
    hash_table->index_entrys = 0;
    hash_table->entrys = (ENTRY**)malloc(sizeof(ENTRY*) * hash_table->size_entrys);

    if (hash_table->entrys == NULL) 
        goto entrys_cleanup;

    if (pthread_mutex_init(&hash_table->mutex, NULL) != 0)
        goto mutex_cleanup;

    return hash_table;

    // Zona dedicata ai cleanup in caso di errori
    mutex_cleanup:
        free(hash_table->entrys);

    entrys_cleanup:
        free(hash_table);

    hash_table_cleanup: 
        hdestroy();

    exit(1);
}

int hash_table_insert(hash_table_t *hash_table, char *key) {
    int *data = (int*)malloc(sizeof(int));
    check(data == NULL, "Errore allocazione int", exit(1));
    *data = 0;

    // Mutex usato per gestire la muta esclusinone sul hash map
    check(pthread_mutex_lock(&hash_table->mutex) != 0, "Errore lock insert hash table", exit(1));

    // Ricerca nella hash table se esiste o meno questa entry
    ENTRY *entry_ptr = hsearch((ENTRY){key, data}, ENTER);
    check(entry_ptr == NULL, "Errore ricerca in hash table", exit(1));

    // In caso non esista viene aggiunta all'hash table ed inoltre viene salvata nell'array 
    // entrys usato per contare il numero di entrys inserite e per fare la deallocazione
    if (*((int*)entry_ptr->data) == 0){

        *((int*)(entry_ptr->data)) = 1;
        hash_table->entrys[hash_table->index_entrys] = entry_ptr;

        hash_table->index_entrys += 1;
        if (hash_table->index_entrys >= hash_table->size_entrys) {
            hash_table->size_entrys *= 2;
            hash_table->entrys = realloc(hash_table->entrys, sizeof(ENTRY*) * hash_table->size_entrys);
        }

    } else {
        // In caso invece già esiste il valore sarà aumentato e sarà eliminata la entry che è stata creata precedentemente
        *((int*)(entry_ptr->data)) += 1;
        free(key);
        free(data);
    }

    //fprintf(stderr, "Insert entry: %s --> %d\n", entry_ptr->key, *((int*)entry_ptr->data));

    // Si fa l'unlock sul mutex per hash table
    check(pthread_mutex_unlock(&hash_table->mutex) != 0, "Errore unlock insert hash table", exit(1));

    return 0;
}

int hash_table_count(hash_table_t *hash_table, char *key) {
    int result = 0;

    // Gestione muta esclusione sull'hash table
    check(pthread_mutex_lock(&hash_table->mutex) != 0, "Errore unlock count hash table", exit(1));

    // Ricerca chiave nell'hash table, in caso venga trovata ritorna il valore di data ed in caso contrario
    // si mantiene il valore 0 su result che verrà ritornato
    ENTRY *entry = hsearch((ENTRY){key, NULL}, FIND);
    if (entry != NULL) result = *((int*)(entry->data));
    
    check(pthread_mutex_unlock(&hash_table->mutex) != 0, "Errore unlock count hash table", exit(1));
    
    return result;
}

void hash_table_destroy(hash_table_t *hash_table) {
    for (unsigned int i = 0; i < hash_table->index_entrys; i++){
        free(hash_table->entrys[i]->data);
        free(hash_table->entrys[i]->key);
    }
    hdestroy();
    check(pthread_mutex_destroy(&hash_table->mutex) != 0, "Errore destroy", exit(1));
    free(hash_table->entrys);
    free(hash_table);
}