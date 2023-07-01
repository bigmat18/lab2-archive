#include <search.h>
#define NUM_ELEM 1000000

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

typedef struct {
    int hash_table_id;
    unsigned int size_entrys;
    unsigned int index_entrys;
    ENTRY **entrys;
} hash_table_t;

hash_table_t *hash_table_create();

int hash_table_insert(hash_table_t *hash_table, const char *key);

void hash_table_destroy(hash_table_t *hash_table);

#endif