#define _GNU_SOURCE
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include "thread.h"

#define HOST "127.0.0.1"
#define PORT 58449

#define Max_sequence_length 2048

#define check(val, str, result)                                                          \
    if (val) {                                                                           \
        fprintf(stderr, "== %s == Linea: %d, File: %s\n", str, __LINE__, __FILE__);      \
        result;                                                                          \
    }


void *tbody(void *args){
  FILE *file = (FILE*)args;
  char *buffer = NULL;

  int fd_skt = 0, tmp;
  struct sockaddr_in serv_addr;

  check((fd_skt = socket(AF_INET, SOCK_STREAM, 0)) < 0, "Errore creazione socket", exit(1));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);
  serv_addr.sin_addr.s_addr = inet_addr(HOST);

  check(connect(fd_skt, &serv_addr, sizeof(serv_addr)) < 0, "Errore connessione", exit(1));

  size_t n = 0;
  ssize_t e;

  char type = 'b';
  check(write(fd_skt, &type, sizeof(type)) != sizeof(type), "Errore write 1", exit(1));

  while (true){
    e = getline(&buffer, &n, file);
    if (e < 0) break;

    fprintf(stderr, "%zd - %s", e, buffer);

    tmp = htonl((int)(strlen(buffer)));
    check(write(fd_skt, &tmp, sizeof(tmp)) != sizeof(tmp), "Errore write 2", exit(1));

    for (unsigned int i = 0; i < strlen(buffer); ++i)
      check(write(fd_skt, &buffer[i], 1) != sizeof(char), "Errore write 3", exit(1));
  }

  tmp = 0;
  check(write(fd_skt, &tmp, sizeof(tmp)) != sizeof(tmp), "Errore write 4", exit(1));

  check(close(fd_skt) < 0, "Errore chiusura socket", exit(1));
  fclose(file);
}

int main(int argv, char **argc){
  assert(argv >= 2);
  thread_t *threads[argv-1];

  for (int i = 1; i < argv; ++i){
    printf("%s\n", argc[i]);
    FILE *file = fopen(argc[i], "r");
    threads[i] = thread_create(file, &tbody);
  }

  for (int i = 0; i < argv-1; ++i){
    pthread_join(threads[i]->thread, NULL);
  }

  return 0;
}