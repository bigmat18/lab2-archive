#define _GNU_SOURCE
#include <assert.h>
#include <string.h>
#include "connection.h"
#include "thread.h"

#define Max_sequence_length 2048

void *tbody(void *args){
  FILE *file = (FILE*)args;
  unsigned short int tmp;

  connection_t *connection = connection_create();

  ssize_t e;
  size_t n;
  char *buffer = NULL;
  char type = 'b';
  check(write(connection->fd_skt, &type, sizeof(type)) != sizeof(type), "Errore write 1", exit(1));

  while((e = getline(&buffer, &n, file)) >= 0) {

    check(e >= Max_sequence_length, "Sequenza di byte troppo lunga", exit(1));
    fprintf(stderr, "%zd - %s", e, buffer);

    tmp = htons(e);
    check(write(connection->fd_skt, &tmp, sizeof(tmp)) != sizeof(tmp), "Errore write 2", exit(1));

    check(write(connection->fd_skt, buffer, e) != e, "Errore write 3", exit(1));
    free(buffer);
    buffer = NULL;
  }

  tmp = 0;
  check(write(connection->fd_skt, &tmp, sizeof(tmp)) != sizeof(tmp), "Errore write 4", exit(1));

  connection_destroy(connection);
  fclose(file);

  pthread_exit(NULL);
}

int main(int argv, char **argc){
  assert(argv >= 2);
  thread_t *threads[argv-1];

  for (int i = 1; i < argv; ++i){
    FILE *file = fopen(argc[i], "r");
    check(file == NULL, "Errore apertura file", close(1));
    threads[i-1] = thread_create(file, &tbody);
  }

  for (int i = 0; i < argv-1; ++i)
    pthread_join(threads[i]->thread, NULL);

  printf("\nDeallocazion in corso...\n");
  for (int i = 0; i < argv-1; ++i)
    thread_destroy(threads[i]);

  return 0;
}
