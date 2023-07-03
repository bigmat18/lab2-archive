#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"

hash_table_t *hash_table_create() {
    int id = hcreate(NUM_ELEM);
    check(id == 0, "Errore nella creazione dell'hash table", pclose(1))

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

    mutex_cleanup:
        free(hash_table->entrys);

    entrys_cleanup:
        free(hash_table);

    hash_table_cleanup: 
        hdestroy();

    pclose(1);
}

int hash_table_insert(hash_table_t *hash_table, const char *key) {
    ENTRY *entry = (ENTRY*)malloc(sizeof(ENTRY)), *entry_ptr;
    check(entry == NULL, "Errore allocazione entry", pclose(1));

    entry->key = strdup(key);
    entry->data = (int*)malloc(sizeof(int));
    check(entry->data == NULL, "Errore allocazione int", pclose(1));

    *((int*)(entry->data)) = 0;

    check(pthread_mutex_lock(&hash_table->mutex) != 0, "Errore lock insert hash table", pclose(1));

    entry_ptr = hsearch(*entry, ENTER);
    check(entry_ptr == NULL, "Errore ricerca in hash table", pclose(1));

    if (*((int*)entry_ptr->data) == 0){

        *((int*)(entry_ptr->data)) = 1;
        hash_table->entrys[hash_table->index_entrys] = entry_ptr;

        hash_table->index_entrys += 1;
        if (hash_table->index_entrys >= hash_table->size_entrys) {
            hash_table->size_entrys *= 2;
            hash_table->entrys = realloc(hash_table->entrys, sizeof(ENTRY*) * hash_table->size_entrys);
        }

    } else *((int*)(entry_ptr->data)) += 1;

    fprintf(stderr, "Insert entry: %s --> %d\n", entry_ptr->key, *((int*)entry_ptr->data));

    check(pthread_mutex_unlock(&hash_table->mutex) != 0, "Errore unlock insert hash table", pclose(1));

    return 0;
}

int hash_table_count(hash_table_t *hash_table, char *key) {
    int result = 0;

    check(pthread_mutex_lock(&hash_table->mutex) != 0, "Errore unlock count hash table", pclose(1));

    ENTRY *entry = hsearch((ENTRY){key, NULL}, FIND);

    if (entry != NULL) result = *((int*)(entry->data));
    check(pthread_mutex_unlock(&hash_table->mutex) != 0, "Errore unlock count hash table", pclose(1));
    
    return result;
}

void hash_table_destroy(hash_table_t *hash_table) {
    for (unsigned int i = 0; i < hash_table->index_entrys; i++){
        free(hash_table->entrys[i]->data);
        free(hash_table->entrys[i]->key);
    }
    hdestroy();
    free(hash_table);
    free(hash_table->entrys);
}