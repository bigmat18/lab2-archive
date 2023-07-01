#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"

hash_table_t *hash_table_create() {
    int id;

    if ((id = hcreate(NUM_ELEM)) == 0) {
        perror("Errore nella creazione dell'hash table");
        return NULL;
    }

    hash_table_t *hash_table = (hash_table_t*)malloc(sizeof(hash_table_t));

    if (hash_table == NULL) {
        perror("Errore allocazione hash table");
        hdestroy();
        return NULL;
    }

    hash_table->hash_table_id = id;
    hash_table->size_entrys = 100;
    hash_table->index_entrys = 0;
    hash_table->entrys = (ENTRY**)malloc(sizeof(ENTRY*) * hash_table->size_entrys);

    if (hash_table->entrys == NULL) {
        perror("Errore allocazione entrys array");
        hdestroy();
        free(hash_table);
        return NULL;
    }

    return hash_table;
}

int hash_table_insert(hash_table_t *hash_table, const char *key) {
    ENTRY *entry = (ENTRY*)malloc(sizeof(ENTRY)), *entry_ptr;
    if (entry == NULL) {
        perror("Errore allocazione entry");
        return 1;
    }

    entry->key = strdup(key);
    entry->data = (int*)malloc(sizeof(int));

    if (entry->data == NULL) {
        perror("Errore allocazione int");
        return 1;
    }
    *((int*)(entry->data)) = 0;

    entry_ptr = hsearch(*entry, ENTER);

    if (entry_ptr == NULL) {
        perror("Errore ricerca in hash table");
        return 1;
    } else if (*((int*)entry_ptr->data) == 0){
        *((int*)(entry_ptr->data)) = 1;
        hash_table->entrys[hash_table->index_entrys] = entry_ptr;

        hash_table->index_entrys += 1;
        if (hash_table->index_entrys >= hash_table->size_entrys) {
            hash_table->size_entrys *= 2;
            hash_table->entrys = realloc(hash_table->entrys, sizeof(ENTRY*) * hash_table->size_entrys);
        }
    } else *((int*)(entry_ptr->data)) += 1;

    fprintf(stderr, "Insert entry: %s --> %d\n", entry_ptr->key, *((int*)entry_ptr->data));

    return 0;
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