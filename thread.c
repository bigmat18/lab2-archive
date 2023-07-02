#include <stdlib.h>
#include "thread.h"

thread_t *thread_create(void *data, void *(*tbody)(void *)) {
    thread_t *thread = (thread_t *)malloc(sizeof(thread_t));
    check(thread == NULL, "Errore allocazione thread", exit(1));

    thread->data = data;
    thread->tbody = tbody;

    check(pthread_create(&thread->thread, NULL, thread->tbody, thread->data), 
          "Errore creazione thread", 
          exit(1));

    return thread;
}

void thread_destroy(thread_t *thread) {
    free(thread->data);
    free(thread);
}