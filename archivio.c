#include "utils.h"
#include "hash_table.h"
#include "thread.h"

#define TOKENIZATOR ".,:; \n\r\t"

void *tbody_prod(void *args);

void *tbody_cons_writer(void *args);

void *tbody_cons_reader(void *args);


int main(int argc, char **argv){
  assert(argc == 3);
  int num_writers = atoi(argv[1]);
  int num_readers = atoi(argv[2]);

  hash_table_t *hash_table = hash_table_create();

  int caposc = open("caposc", O_RDONLY);
  int capolet = open("capolet", O_RDONLY);

  buffer_t *buffer_writer = buffer_create();
  buffer_t *buffer_reader = buffer_create();

  data_t *data_writer = (data_t*)malloc(sizeof(data_t));
  data_writer->buffer = buffer_writer;
  data_writer->pipe = caposc;
  data_writer->hash_table = hash_table;
  data_writer->file = NULL;

  data_t *data_reader = (data_t *)malloc(sizeof(data_t));
  data_reader->buffer = buffer_reader;
  data_reader->pipe = capolet;
  data_reader->hash_table = hash_table;
  data_writer->file = fopen("lettori.log", "w");


  thread_t *prod_writer = thread_create(data_writer, &tbody_prod);
  thread_t *cons_writer[num_writers];

  for(int i=0; i<num_writers; i++)
    cons_writer[i] = thread_create(data_writer, &tbody_cons_writer);


  thread_t *prod_reader = thread_create(data_reader, &tbody_prod);
  thread_t *cons_reader[num_readers];

  for (int i = 0; i < num_readers; i++)
    cons_reader[i] = thread_create(data_reader, &tbody_cons_reader);


  pthread_join(prod_writer->thread, NULL);

  for(int i=0; i<num_writers; i++)
    pthread_join(cons_writer[i]->thread, NULL);

  pthread_join(prod_reader->thread, NULL);

  for (int i = 0; i < num_readers; i++)
    pthread_join(cons_reader[i]->thread, NULL);


  fclose(data_reader->file);
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
    size_t e = readn(data->pipe, &n, sizeof(n));
    check(e != sizeof(n), "Errore in reading 1", exit(1));

    temp_buf = (char*)malloc(n * sizeof(char));

    e = readn(data->pipe, temp_buf, n);
    check(e != n, "Errore in reading 1", exit(1));

    char *token = strtok(temp_buf, TOKENIZATOR);

    while (token != NULL) {
      buffer_insert(data->buffer, strdup(token));
      token = strtok(NULL, TOKENIZATOR);
    }

  } while (true); 
  free(temp_buf);
}

void* tbody_cons_writer(void* args){
  data_t *data = (data_t *)args;
  do {
    hash_table_insert(data->hash_table, buffer_remove(data->buffer, NULL));
  } while (true);
}

void *tbody_cons_reader(void *args){
  data_t *data = (data_t *)args;
  do {
    buffer_remove(data->buffer, data->file);
  } while(true);
}