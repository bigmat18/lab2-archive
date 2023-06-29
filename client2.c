#define _GNU_SOURCE
#include <arpa/inet.h>
#include <sys/socket.h>
#include "utils.h"

#define HOST "127.0.0.1"
#define PORT 58449
#define QUI __LINE__, __FILE__

typedef struct{
  FILE *file;
} Data;

void *tbody(void *args){
  Data *data = (Data*)args;
  char *buffer = NULL;

  int fd_skt = 0, tmp;
  struct sockaddr_in serv_addr;

  printf("fai schifo1\n");

  if ((fd_skt = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    termina("Errore creazione socket");

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);
  serv_addr.sin_addr.s_addr = inet_addr(HOST);

  if (connect(fd_skt, &serv_addr, sizeof(serv_addr)) < 0)
    termina("Errore apertura connessione");

  size_t n = 0;
  ssize_t e;

  printf("fai schifo2\n");

  char type = 'b';
  e = writen(fd_skt, &type, sizeof(type));
  if (e != sizeof(char))
    termina("Errore write 1");

  printf("fai schifo3\n");

  while (true){
    e = getline(&buffer, &n, data->file);
    if (e < 0)
      break;

    printf("%zd - %s", e, buffer);

    tmp = htonl((int)(strlen(buffer)));
    e = writen(fd_skt, &tmp, sizeof(tmp));
    if (e != sizeof(int))
      termina("Errore write 2");

    for (unsigned int i = 0; i < strlen(buffer); ++i){
      e = writen(fd_skt, &buffer[i], 1);
      if (e != 1)
        termina("Errore write 3");
    }
  }

  tmp = 0;
  e = writen(fd_skt, &tmp, sizeof(tmp));
  if (e != sizeof(int))
    termina("Errore write");

  if (close(fd_skt) < 0)
    perror("Errore chiusura socket");
  
  fclose(data->file);
}

int main(int argv, char **argc){
  if (argv < 2) termina("Inviare file");

  pthread_t threads[argv-1];
  Data data[argv-1];

  for (int i = 1; i < argv; ++i){
    printf("%s\n", argc[i]);
    FILE *file = fopen(argc[i], "r");
    data[i-1].file = file;
    xpthread_create(&threads[i-1], NULL, &tbody, &data[i-1], QUI);
  }

  for (int i = 0; i < argv-1; ++i){
    xpthread_join(threads[i], NULL, QUI);
  }

  return 0;
}