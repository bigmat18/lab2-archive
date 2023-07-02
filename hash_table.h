#include <search.h>

#ifndef check(val, str, result)
    #define check(val, str, result) \
        if (val)                    \
            perror(str);            \
        result;
#endif check(val, str, result)

#define NUM_ELEM 1000000

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

typedef struct {
    int hash_table_id;          // id of hash_table from hcreate
    unsigned int size_entrys;   // size of entrys array
    unsigned int index_entrys;  // actual index in entrys array
    ENTRY **entrys;
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
int hash_table_insert(hash_table_t *hash_table, const char *key);


/**
 * @details Free memory from all data in a hash_table_t
 *
 * @param *hash_table: pointer to hash table
 */
void hash_table_destroy(hash_table_t *hash_table);

#endif