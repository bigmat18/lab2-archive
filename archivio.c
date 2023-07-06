#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdbool.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include "hash_table.h"
#include "thread.h"

// Stringa usata per tokenizare
#define TOKENIZATOR ".,:; \n\r\t"

// Variabile 'volatile' per stoppare i thread nel momento in cui viene mandato il segnale SIGTERM
volatile bool interrupt = false;


// ------ Definizioni funzioni usati dai thread -------
void *tbody_prod(void *args);

void *tbody_cons_writer(void *args);

void *tbody_cons_reader(void *args);

void *tbody_signals_handler(void *args);
// ------ Definizioni funzioni usati dai thread -------


int main(int argc, char **argv){
  assert(argc == 3);
  int num_writers = atoi(argv[1]);
  int num_readers = atoi(argv[2]);

  // Inizzializzaioni informazioni per i segnali
  sigset_t mask;
  sigfillset(&mask);
  sigdelset(&mask, SIGQUIT);
  sigdelset(&mask, SIGTERM);
  pthread_sigmask(SIG_BLOCK, &mask, NULL);

  // Creazioni hash_table
  hash_table_t *hash_table = hash_table_create();

  // Creazione thread che gestisce segnali
  thread_t *signals_handler = thread_create(hash_table, &tbody_signals_handler);

  // Apertura pipe e file utili
  int caposc = open("caposc", O_RDONLY);
  int capolet = open("capolet", O_RDONLY);
  FILE *file = fopen("lettori.log", "a");
  check(file == NULL, "Errore apertura file", exit(1));

  // Creazione dei buffer usati dagli scrittori e dai lettori
  buffer_t *buffer_writer = buffer_create();
  buffer_t *buffer_reader = buffer_create();

  // Inizializzazione dei dati dai inviare ai scrittori
  data_t *data_writer = (data_t*)malloc(sizeof(data_t));
  data_writer->buffer = buffer_writer;
  data_writer->pipe = caposc;
  data_writer->hash_table = hash_table;
  data_writer->file = NULL;

  // Inizializzazione dei dati dai inviare ai lettori
  data_t *data_reader = (data_t *)malloc(sizeof(data_t));
  data_reader->buffer = buffer_reader;
  data_reader->pipe = capolet;
  data_reader->hash_table = hash_table;
  data_reader->file = file;

  // Creazione del thread capo (produttore) e dei thread consumatori che si occupano della scrittura
  thread_t *prod_writer = thread_create(data_writer, &tbody_prod);
  thread_t *cons_writer[num_writers];

  for(int i=0; i<num_writers; i++)
    cons_writer[i] = thread_create(data_writer, &tbody_cons_writer);


  // Creazione del thread capo (produttore) e dei thread consumatori che si occupano della lettura
  thread_t *prod_reader = thread_create(data_reader, &tbody_prod);
  thread_t *cons_reader[num_readers];

  for (int i = 0; i < num_readers; i++)
    cons_reader[i] = thread_create(data_reader, &tbody_cons_reader);


  // Attesa di tutti i thread creati
  pthread_join(signals_handler->thread, NULL);
  pthread_join(prod_writer->thread, NULL);

  for(int i=0; i<num_writers; i++)
    pthread_join(cons_writer[i]->thread, NULL);

  pthread_join(prod_reader->thread, NULL);

  for (int i = 0; i < num_readers; i++)
    pthread_join(cons_reader[i]->thread, NULL);


  // Chiusura file e connessioni a pipe e deallocazioni delle strutture utilizzate
  fprintf(stderr, "Terminazione in corso...\n");
  fclose(file);
  close(caposc);
  close(capolet);

  buffer_destroy(buffer_writer);
  buffer_destroy(buffer_reader);
  hash_table_destroy(hash_table);

  free(data_writer);
  free(data_reader);
  return 0;
}

void* tbody_prod(void *args){
  data_t *data = (data_t*)args;
  int n;
  char *temp_buf;

  do {
    // Lettura dalla pipe della lunghezza della stringa da leggere
    size_t e = read(data->pipe, &n, sizeof(n));
    check(e != sizeof(n), "Errore in reading 1", exit(1));

    // Allocazione buffere dove scrivere la seguenza di caratteri
    temp_buf = (char*)malloc(n * sizeof(char));

    // Lettura dalla pipe della seguneza di caratteri
    check(read(data->pipe, temp_buf, n) != n, "Errore in reading 2", exit(1));

    // Prima tokenizzazione seguenza caratteri
    char *token = strtok(temp_buf, TOKENIZATOR);

    // Inserimento nel buffer
    while (token != NULL) {
      buffer_insert(data->buffer, strdup(token));
      token = strtok(NULL, TOKENIZATOR);
    }

  } while (!interrupt); 

  // Deallocazione buffer temporaneo
  free(temp_buf);
  pthread_exit(NULL);
}

void* tbody_cons_writer(void* args){
  data_t *data = (data_t *)args;
  do {
    hash_table_insert(data->hash_table, buffer_remove(data->buffer));
  } while (!interrupt);
  pthread_exit(NULL);
}

void *tbody_cons_reader(void *args){
  data_t *data = (data_t *)args;
  do {
    char* key = buffer_remove(data->buffer);
    int num = hash_table_count(data->hash_table, key);

    fprintf(stderr, "%s %d\n", key, num);
    fprintf(data->file, "%s %d\n", key, num);
    fflush(data->file);

  } while(!interrupt);
  pthread_exit(NULL);
}

void *tbody_signals_handler(void *args) {
  hash_table_t *data = (hash_table_t*)args;
  sigset_t mask;
  sigfillset(&mask);
  int s;

  while(!interrupt) {
    check(sigwait(&mask,&s) != 0, "Errore sigwait", exit(1));

    if (s == SIGINT) {
      check(pthread_mutex_lock(&data->mutex) != 0, "Errore mutex lock in signals handler", exit(1));
      fprintf(stderr, "Numero stringhe in hash map: %d\n", data->index_entrys);
      check(pthread_mutex_unlock(&data->mutex) != 0, "Errore mutex lock in signals handler", exit(1));
    }

    if (s == SIGTERM) {
      interrupt = true;
      check(pthread_mutex_lock(&data->mutex) != 0, "Errore mutex lock in signals handler", exit(1));
      fprintf(stdout, "Numero stringhe in hash map: %d\n", data->index_entrys);
      check(pthread_mutex_lock(&data->mutex) != 0, "Errore mutex lock in signals handler", exit(1));
    }

  }
  pthread_exit(NULL);
}