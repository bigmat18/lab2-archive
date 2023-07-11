#include <stdlib.h>
#include "thread.h"

thread_t *thread_create(void *data, void *(*tbody)(void *)) {
    // Allocazione struttura thread
    thread_t *thread = (thread_t *)malloc(sizeof(thread_t));
    check(thread == NULL, "Errore allocazione thread", exit(1));

    thread->data = data;
    thread->tbody = tbody;

    // Creazione e "partenza" del thread
    check(pthread_create(&thread->thread, NULL, thread->tbody, thread->data), 
          "Errore creazione thread", 
          exit(1));

    // Ritorno puntatore alla struttura del thread
    return thread;
}

void thread_destroy(thread_t *thread) {
    free(thread);
}