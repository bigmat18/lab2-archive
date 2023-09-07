#define _GNU_SOURCE 
#include <stdbool.h> 
#include <assert.h> 
#include <string.h> 
#include <unistd.h>
#include "connection.h"

#define Max_sequence_length 2048

int main(int argv, char** argc){
  assert(argv == 2);

  // Apertura file
  FILE *file = fopen(argc[1], "r");
  check(file == NULL, "Errore apertura file", exit(1));

  // Definizioni variabili generali
  connection_t *connection = NULL;
  char type = 'a';
  ssize_t e;
  size_t n;
  unsigned short int tmp;
  char *buffer = NULL;

  // Ciclo che prende una line avvia una connessione di tipo A ed invia i dati (prima lunghezza e poi buffer)
  while ((e = getline(&buffer, &n, file)) >= 0) {
    connection = connection_create();
    check(writeN(connection->fd_skt, &type, sizeof(type)) != sizeof(type), "Errore write 1", exit(1));

    check(e > Max_sequence_length, "Sequenza di byte troppo lunga", exit(1));

    tmp = htons(e);
    check(writeN(connection->fd_skt, &tmp, sizeof(tmp)) != sizeof(tmp), "Errore write 2", exit(1));

    check(writeN(connection->fd_skt, buffer, e) != e, "Errore write 3", exit(1));
    // fprintf(stderr, "%zd - %s", e, buffer);

    // Pulizia buffer per ciclo successiovo
    free(buffer);
    buffer = NULL;

    // Chiusura connessione
    connection_destroy(connection);
  }

  if (buffer != NULL) free(buffer);

  // printf("\nDeallocazion in corso...\n");
  // Chiusura file
  fclose(file);

  return 0;
}