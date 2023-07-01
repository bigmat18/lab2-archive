#include <pthread.h>

#ifndef BUFFER_H
#define BUFFER_H

typedef struct {
    char **buffer;

    pthread_cond_t empty;
    pthread_cond_t full;
    pthread_mutex_t mutex;

    int index, pindex, cindex;
} buffer_t;



#endif
