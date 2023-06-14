#define _GNU_SOURCE  /* See feature_test_macros(7) */
#include <stdio.h>   // permette di usare scanf printf etc ...
#include <stdlib.h>  // conversioni stringa/numero exit() etc ...
#include <stdbool.h> // gestisce tipo bool (variabili booleane)
#include <assert.h>  // permette di usare la funzione assert
#include <string.h>  // confronto/copia/etc di stringhe
#include <errno.h>
#include <search.h>
#include <signal.h>
#include <unistd.h> // per sleep

#define NUM_ELEM 1000000
#define PC_BUFFER_LEN 10

typedef struct {
    int data;
    ENTRY *next;
} Data;

void terminate(const char *message){
  if(errno!=0) perror(message);
	else fprintf(stderr,"%s\n", message);
  exit(1);
}

ENTRY *create_entry(char *s, int n) {
  ENTRY *e = malloc(sizeof(ENTRY));

  if(e==NULL) terminate("error malloc entry 1");
  e->key = strdup(s);
  e->data = (Data*)malloc(sizeof(Data));

  if(e->key==NULL || e->data==NULL)
    terminate("error malloc entry 2");

  ((Data*)e->data)->data = n;
  ((Data*)e->data)->next = NULL;
  return e;
}

void remove_entry(ENTRY *e) {
  free(e->key); 
  free(e->data); 
  free(e);
}

void aggiungi(char *s){

}

int conta(char *s){

}

int main(int argc, char **argv){
    assert(argc>2);
    int num_writers = atoi(argv[1]);
    int num_readers = atoi(argv[2]);

    int hash_table = hcreate(NUM_ELEM);
    if(hash_table == 0) terminate("Error creation hash table");
    printf("daje");
    return 0;
}
