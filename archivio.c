#define _GNU_SOURCE
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

// Variabile per stoppare i thread nel momento in cui viene mandato il segnale SIGTERM
bool interrupt = false;


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
  pthread_sigmask(SIG_BLOCK, &mask, NULL);

  // Inizzializzaione del mutex che gestisce la variabile globale per interropere i thread
  pthread_mutex_t interrupt_mutex;
  check(pthread_mutex_init(&interrupt_mutex, NULL) != 0, "Errore creazione mutex interrupt", exit(1));

  // Creazioni hash_table
  hash_table_t *hash_table = hash_table_create();

  // Creazione data da mandare al thread che gestisce i i segnali
  handler_data_t *data_handler = (handler_data_t*)malloc(sizeof(handler_data_t));
  data_handler->hash_table = hash_table;
  data_handler->interrupt_mutex = &interrupt_mutex;

  // Creazione thread che gestisce segnali
  thread_t *signals_handler = thread_create(data_handler, &tbody_signals_handler);

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
  check(data_writer == NULL, "Errore allocazione data writer", exit(1));
  data_writer->buffer = buffer_writer;
  data_writer->pipe = caposc;
  data_writer->hash_table = hash_table;
  data_writer->interrupt_mutex = &interrupt_mutex;
  data_writer->file = NULL;

  // Inizializzazione dei dati dai inviare ai lettori
  data_t *data_reader = (data_t *)malloc(sizeof(data_t));
  check(data_reader == NULL, "Errore allocazione data reader", exit(1));
  data_reader->buffer = buffer_reader;
  data_reader->pipe = capolet;
  data_reader->hash_table = hash_table;
  data_reader->interrupt_mutex = &interrupt_mutex;
  data_reader->file = file;

  // Inizializzazione mutex per gestire la scrittura su file
  check(pthread_mutex_init(&data_reader->file_mutex, NULL) != 0, "Errore creazione mutex interrupt", exit(1));

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

  thread_destroy(signals_handler);
  thread_destroy(prod_reader);
  thread_destroy(prod_writer);

  for(int i=0; i<num_writers; i++)
    thread_destroy(cons_writer[i]);


  for (int i = 0; i < num_readers; i++)
    thread_destroy(cons_reader[i]);

  fclose(file);
  close(caposc);
  close(capolet);

  buffer_destroy(buffer_writer);
  buffer_destroy(buffer_reader);
  hash_table_destroy(hash_table);

  free(data_writer);
  free(data_reader);
  free(data_handler);
  return 0;
}

void* tbody_prod(void *args){
  data_t *data = (data_t*)args;
  unsigned short int n;
  char *temp_buf = NULL;
  char *token;

  do {
    // Gestione muta eslusione della variable interrupt
    check(pthread_mutex_unlock(data->interrupt_mutex) != 0, "Errore unlock interrupt", pthread_exit(NULL));

    // Lettura dalla pipe della lunghezza della stringa da leggere
    size_t e = read(data->pipe, &n, sizeof(n));
    check(e != sizeof(n), "Errore in reading 1", pthread_exit(NULL));

    // Allocazione buffere dove scrivere la seguenza di caratteri
    temp_buf = (char*)malloc(n * sizeof(char));

    // Lettura dalla pipe della seguneza di caratteri
    check(read(data->pipe, temp_buf, n) != n, "Errore in reading 2", pthread_exit(NULL));

    // Inserimento nel buffer
    while ((token = strtok_r(temp_buf, TOKENIZATOR, &temp_buf)) != NULL){
      buffer_insert(data->buffer, strdup(token));
    }

    // Deallocazione buffer temporaneo
    free(temp_buf);
    temp_buf = NULL;

    // Gestione muta eslusione della variable interrupt
    check(pthread_mutex_lock(data->interrupt_mutex) != 0, "Errore lock interrupt", pthread_exit(NULL));
  } while (!interrupt); 

  pthread_exit(NULL);
}

void* tbody_cons_writer(void* args){
  data_t *data = (data_t *)args;
  do {
    // Gestione muta eslusione della variable interrupt
    check(pthread_mutex_unlock(data->interrupt_mutex) != 0, "Errore unlock interrupt", pthread_exit(NULL));

    // Si prende l'elemento dal buffer e si inserisce nell'hash table
    hash_table_insert(data->hash_table, buffer_remove(data->buffer));

    // Gestione muta eslusione della variable interrupt
    check(pthread_mutex_lock(data->interrupt_mutex) != 0, "Errore lock interrupt", pthread_exit(NULL));
  } while (!interrupt);
  pthread_exit(NULL);
}

void *tbody_cons_reader(void *args){
  data_t *data = (data_t *)args;
  do {
    // Gestione muta eslusione della variable interrupt 
    check(pthread_mutex_unlock(data->interrupt_mutex) != 0, "Errore unlock interrupt", pthread_exit(NULL));

    // Si prende la stringa dal buffer
    char* key = buffer_remove(data->buffer);

    // Si prende il numero di occorrenze della stringa
    int num = hash_table_count(data->hash_table, key);

    // Si stampa nel file gestendo anche la muta eslusione 
    check(pthread_mutex_lock(&data->file_mutex) != 0, "Errore lock file", pthread_exit(NULL));
    fprintf(stderr, "%s %d\n", key, num);
    fprintf(data->file, "%s %d\n", key, num);
    fflush(data->file);
    check(pthread_mutex_unlock(&data->file_mutex) != 0, "Errore unlock file", pthread_exit(NULL));

    // Gestione muta eslusione della variable interrupt
    check(pthread_mutex_lock(data->interrupt_mutex) != 0, "Errore lock interrupt", pthread_exit(NULL));
  } while(!interrupt);
  pthread_exit(NULL);
}

void *tbody_signals_handler(void *args) {
  handler_data_t *data = (handler_data_t*)args;
  sigset_t mask;
  sigfillset(&mask);
  sigdelset(&mask, SIGQUIT);
  sigdelset(&mask, SIGTERM);
  int s;

  while(true) {
    check(sigwait(&mask,&s) != 0, "Errore sigwait", pthread_exit(NULL));

    if (s == SIGINT) {
      // Gestione della SIGINT andando a stampare il numero di stringhe nell'hash map gestendo anche la muta eslusione
      check(pthread_mutex_lock(&data->hash_table->mutex) != 0, "Errore mutex lock in signals handler", pthread_exit(NULL));
      fprintf(stderr, "Numero stringhe in hash map: %d\n", data->hash_table->index_entrys);
      check(pthread_mutex_unlock(&data->hash_table->mutex) != 0, "Errore mutex lock in signals handler", pthread_exit(NULL));
    } else if (s == SIGTERM) {

      // Gestionne della SIGTERM andando ad importare interrupt a true (quindi si interrompono i vari thread)
      check(pthread_mutex_lock(data->interrupt_mutex) != 0, "Errore unlock interrupt", pthread_exit(NULL));
      interrupt = true;
      check(pthread_mutex_unlock(data->interrupt_mutex) != 0, "Errore unlock interrupt", pthread_exit(NULL));

      // Stampa del numero di stringhe nell'hash map
      check(pthread_mutex_lock(&data->hash_table->mutex) != 0, "Errore mutex lock in signals handler", pthread_exit(NULL));
      fprintf(stdout, "Numero stringhe in hash map: %d\n", data->hash_table->index_entrys);
      check(pthread_mutex_unlock(&data->hash_table->mutex) != 0, "Errore mutex lock in signals handler", pthread_exit(NULL));

      break;
    }

  }
  pthread_exit(NULL);
}