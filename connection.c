#include "connection.h"

connection_t *connection_create() {
    connection_t *connection = (connection_t*)malloc(sizeof(connection_t));
    check(connection == NULL, "Errore allocazione connection", exit(1));

    check((connection->fd_skt = socket(AF_INET, SOCK_STREAM, 0)) < 0, "Errore creazione socket", exit(1));

    connection->serv_addr.sin_family = AF_INET;
    connection->serv_addr.sin_port = htons(PORT);
    connection->serv_addr.sin_addr.s_addr = inet_addr(HOST);

    check(connect(connection->fd_skt, &connection->serv_addr, sizeof(connection->serv_addr)) < 0, "Errore connessione", exit(1));
    return connection;
}

void connection_destroy(connection_t *connection) {
    check(close(connection->fd_skt) < 0, "Errore chiusura socket", exit(1));
    free(connection);
}