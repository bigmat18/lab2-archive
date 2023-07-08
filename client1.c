#define _GNU_SOURCE 
#include <stdio.h> 
#include <stdlib.h>
#include <stdbool.h> 
#include <assert.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define HOST "127.0.0.1"
#define PORT 58449
#define Max_sequence_length 2048

#define check(val, str, result)                                                          \
    if (val) {                                                                           \
        fprintf(stderr, "== %s == Linea: %d, File: %s\n", str, __LINE__, __FILE__);      \
        result;                                                                          \
    }


int main(int argv, char** argc){
  assert(argv == 2);

  FILE *file = fopen(argc[1], "r");
  check(file == NULL, "Errore apertura file", exit(1));

  int fd_skt = 0;
  struct sockaddr_in serv_addr;

  check((fd_skt = socket(AF_INET, SOCK_STREAM, 0)) < 0, "Errore creazione socket", exit(1));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);
  serv_addr.sin_addr.s_addr = inet_addr(HOST);

  check(connect(fd_skt, &serv_addr, sizeof(serv_addr)) < 0, "Errore connessione", exit(1));

  ssize_t e;
  size_t n;
  unsigned short int num_byte = 0, tmp;
  char *temp_buffer = NULL;
  char *buffer = (char*)malloc(num_byte);

  char type = 'a';
  check(write(fd_skt, &type, sizeof(type)) != sizeof(type),"Errore write 1", exit(1));

  while ((e = getline(&temp_buffer, &n, file)) >= 0) {
    num_byte += e;

    buffer = realloc(buffer, num_byte);
    strcat(buffer, temp_buffer);

    free(temp_buffer);
    temp_buffer = NULL;
  }

  check(num_byte >= Max_sequence_length, "Sequenza di byte troppo lunga", exit(1));

  tmp = htons(num_byte);
  check(write(fd_skt, &tmp, sizeof(tmp)) != sizeof(tmp), "Errore write 2", exit(1));

  check(write(fd_skt, buffer, num_byte) != num_byte, "Errore write 3", exit(1));
  fprintf(stderr, "%hu - %lu - %lu - %s", num_byte, sizeof(num_byte), strlen(buffer), buffer);

  check(close(fd_skt) < 0, "Errore chiusura socket", exit(1));  
  fclose(file);

  return 0;
}