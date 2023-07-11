#define _GNU_SOURCE 
#include <stdbool.h> 
#include <assert.h> 
#include <string.h> 
#include <unistd.h>
#include "connection.h"

#define Max_sequence_length 2048

int main(int argv, char** argc){
  assert(argv == 2);

  FILE *file = fopen(argc[1], "r");
  check(file == NULL, "Errore apertura file", exit(1));

  connection_t *connection = NULL;
  char type = 'a';
  ssize_t e;
  size_t n;
  unsigned short int tmp;
  char *buffer = NULL;

  while ((e = getline(&buffer, &n, file)) >= 0) {
    connection = connection_create();
    check(write(connection->fd_skt, &type, sizeof(type)) != sizeof(type), "Errore write 1", exit(1));

    check(e >= Max_sequence_length, "Sequenza di byte troppo lunga", exit(1));

    tmp = htons(e);
    check(write(connection->fd_skt, &tmp, sizeof(tmp)) != sizeof(tmp), "Errore write 2", exit(1));

    check(write(connection->fd_skt, buffer, e) != e, "Errore write 3", exit(1));
    fprintf(stderr, "%zd - %s", e, buffer);

    free(buffer);
    buffer = NULL;
    connection_destroy(connection);
  }

  printf("\nDeallocazion in corso...\n");
  fclose(file);

  return 0;
}