#define _GNU_SOURCE
#include <search.h>
#include <pthread.h>
#include <errno.h>

// Macro creata per controllare gli errori nei valori di ritorno
#define check(val, str, result)                                             \
    if (val) {                                                              \
        if (errno == 0) fprintf(stderr, "== %s\n", str);                    \
        else fprintf(stderr, "== %s: %s\n", str, strerror(errno));          \
        fprintf(stderr, "== Linea: %d, File: %s\n", __LINE__, __FILE__);    \
        result;                                                             \
    }

#define NUM_ELEM 1000000

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

typedef struct {
    int hash_table_id;          // id of hash_table from hcreate
    unsigned int size_entrys;   // size of entrys array
    unsigned int index_entrys;  // actual index in entrys array
    ENTRY **entrys;

    pthread_mutex_t mutex;      // the butex that manage the mutal exclusion in hash table
} hash_table_t;

/**
 * @details Create hash table.
 * 
 * @return Return a pointer to hash_table type
*/
hash_table_t *hash_table_create();


/** 
 * @details Insert element in hash table or if element already exits increment value of data.
 * 
 * @param *hash_table: pointer to hash table
 * @param *key: value of new key
 * 
 * @return return 1 if there is an errors, return 0 without errors
*/
int hash_table_insert(hash_table_t *hash_table, char *key);


/**
 * @details Return the count in the entry with key value
 * 
 * @param *hash_table: pointer to hash table
 * @param *key: the key of the entry
 * 
 * @return return the count if the entry exist and 0 if the entry will not be found
*/
int hash_table_count(hash_table_t *hash_table, char *key);


/**
 * @details Free memory from all data in a hash_table_t
 *
 * @param *hash_table: pointer to hash table
 */
void hash_table_destroy(hash_table_t *hash_table);

#endif