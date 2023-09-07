#include "connection.h"

connection_t *connection_create() {
    // Allocazione connessione
    connection_t *connection = (connection_t*)malloc(sizeof(connection_t));
    check(connection == NULL, "Errore allocazione connection", exit(1));

    // Creazione della connessione
    check((connection->fd_skt = socket(AF_INET, SOCK_STREAM, 0)) < 0, "Errore creazione socket", exit(1));

    connection->serv_addr.sin_family = AF_INET;
    connection->serv_addr.sin_port = htons(PORT);
    connection->serv_addr.sin_addr.s_addr = inet_addr(HOST);

    // Avvio connessione
    check(connect(connection->fd_skt, (const struct sockaddr *)&connection->serv_addr, sizeof(connection->serv_addr)) < 0, "Errore connessione", exit(1));
    return connection;
}

void connection_destroy(connection_t *connection) {
    check(close(connection->fd_skt) < 0, "Errore chiusura socket", exit(1));
    free(connection);
}

ssize_t readN(int fd, void *ptr, size_t n) {  
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

ssize_t writeN(int fd, void *ptr, size_t n) {  
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