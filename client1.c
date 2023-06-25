#define _GNU_SOURCE 
#include <stdio.h> 
#include <stdlib.h>
#include <stdbool.h> 
#include <assert.h> 
#include <string.h> 
#include <errno.h>
#include <search.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define HOST "127.0.0.1"
#define PORT 58449

void terminate(const char *message){
    if (errno != 0) perror(message);
	else fprintf(stderr,"%s\n", message);
  exit(1);
}

ssize_t writen(int fd, void *ptr, size_t n) {  
   size_t   nleft;
   ssize_t  nwritten;
 
   nleft = n;
   while (nleft > 0) {
     if((nwritten = write(fd, ptr, nleft)) < 0) {
        if (nleft == n) return -1;
        else break;
     } else if (nwritten == 0) break; 
     nleft -= nwritten;
     ptr   += nwritten;
   }
   return(n - nleft);
}

ssize_t readn(int fd, void *ptr, size_t n) {  
   size_t   nleft;
   ssize_t  nread;
 
   nleft = n;
   while (nleft > 0) {
     if((nread = read(fd, ptr, nleft)) < 0) {
        if (nleft == n) return -1;
        else break;
     } else if (nread == 0) break;
     nleft -= nread;
     ptr   += nread;
   }
   return(n - nleft);
}

int main(int argv, char** argc){
    if(argv != 2) terminate("Inviare file");

    FILE *file = fopen(argc[1], "r");
    char *buffer = NULL;

    int fd_skt = 0, tmp;
    struct sockaddr_in serv_addr;

    if ((fd_skt = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        terminate("Errore creazione socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(HOST);

    if (connect(fd_skt, &serv_addr, sizeof(serv_addr)) < 0) 
      terminate("Errore apertura connessione");
    size_t n = 0;
    ssize_t e;

    while(true){
        e = getline(&buffer, &n, file);
        if (e < 0) break;

        printf("%zd - %s", e, buffer);

        tmp = htonl((int)(strlen(buffer)));
        e = writen(fd_skt, &tmp, sizeof(tmp));
        if (e != sizeof(int)) terminate("Errore write");

        for(unsigned int i = 0; i < strlen(buffer); ++i){
          e = writen(fd_skt, &buffer[i], 1);
          if (e != 1) terminate("Errore write");
        }

        break;

        // e = writen(fd_skt, &buffer, strlen(buffer));
        // if (e != strlen(buffer)) terminate("Errore write");
        // break;
    }

    if (close(fd_skt) < 0) perror("Errore chiusura socket");
    fclose(file);

    return 0;
}