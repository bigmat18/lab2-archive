#define _GNU_SOURCE
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "thread.h"

#define HOST "127.0.0.1"
#define PORT 58449

#define Max_sequence_length 2048

#define check(val, str, result)                                             \
    if (val) {                                                              \
        if (errno == 0) fprintf(stderr, "== %s\n", str);                    \
        else fprintf(stderr, "== %s: %s\n", str, strerror(errno));          \
        fprintf(stderr, "== Linea: %d, File: %s\n", __LINE__, __FILE__);    \
        result;                                                             \
    }

void *tbody(void *args){
  FILE *file = (FILE*)args;
  int fd_skt = 0;
  unsigned short int tmp;
  struct sockaddr_in serv_addr;

  check((fd_skt = socket(AF_INET, SOCK_STREAM, 0)) < 0, "Errore creazione socket", exit(1));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);
  serv_addr.sin_addr.s_addr = inet_addr(HOST);

  check(connect(fd_skt, &serv_addr, sizeof(serv_addr)) < 0, "Errore connessione", exit(1));

  ssize_t e;
  size_t n;
  char *buffer = NULL;
  char type = 'b';
  check(write(fd_skt, &type, sizeof(type)) != sizeof(type), "Errore write 1", exit(1));

  while((e = getline(&buffer, &n, file)) >= 0) {

    check(e >= Max_sequence_length, "Sequenza di byte troppo lunga", exit(1));
    fprintf(stderr, "%zd - %s", e, buffer);

    tmp = htons(e);
    check(write(fd_skt, &tmp, sizeof(tmp)) != sizeof(tmp), "Errore write 2", exit(1));

    check(write(fd_skt, buffer, e) != e, "Errore write 3", exit(1));
    free(buffer);
    buffer = NULL;
  }

  tmp = 0;
  check(write(fd_skt, &tmp, sizeof(tmp)) != sizeof(tmp), "Errore write 4", exit(1));

  check(close(fd_skt) < 0, "Errore chiusura socket", exit(1));  
  fclose(file);

  pthread_exit(NULL);
}

int main(int argv, char **argc){
  assert(argv >= 2);
  thread_t *threads[argv-1];

  for (int i = 1; i < argv; ++i){
    printf("%s\n", argc[i]);
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
