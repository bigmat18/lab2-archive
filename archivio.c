#include "utils.h"
#include "hash_table.h"
#include "thread.h"

#define TOKENIZATOR ".,:; \n\r\t"

void* tbody_prod_writer(void *args){
  data_writer_t *data = (data_writer_t*)args;
  int n;
  char *temp_buf;

  do {
    size_t e = readn(data->caposc, &n, sizeof(n));
    check(e != sizeof(n), "Errore in reading 1", exit(1));

    temp_buf = (char*)malloc(n * sizeof(char));

    e = readn(data->caposc, temp_buf, n);
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
  data_writer_t *data = (data_writer_t *)args;
  do {
    buffer_consume(data->buffer, data->hash_table);
  } while (true);
}

int main(int argc, char **argv){
  assert(argc == 3);
  int num_writers = atoi(argv[1]);
  // int num_readers = atoi(argv[2]);

  hash_table_t *hash_table = hash_table_create();

  int caposc = open("caposc", O_RDONLY);
  int capolet = open("capolet", O_RDONLY);

  buffer_t *buffer = buffer_create();

  data_writer_t *data_writer = (data_writer_t*)malloc(sizeof(data_writer_t));
  data_writer->buffer = buffer;
  data_writer->caposc = caposc;
  data_writer->hash_table = hash_table;

  thread_t *prod_writer = thread_create(data_writer, &tbody_prod_writer);
  thread_t *cons_writer[num_writers];

  for(int i=0; i<num_writers; i++)
    cons_writer[i] = thread_create(data_writer, &tbody_cons_writer);

  pthread_join(prod_writer->thread, NULL);

  for(int i=0; i<num_writers; i++)
    pthread_join(cons_writer[i]->thread, NULL);

  close(caposc);
  close(capolet);

  buffer_destroy(buffer);
  hash_table_destroy(hash_table);

  free(data_writer);
  return 0;
}