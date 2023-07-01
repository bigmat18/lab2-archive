#include "utils.h"
#include "hash_table.h"

#define QUI __LINE__, __FILE__
#define PC_BUFFER_LEN 10

hash_table_t *HASH_TABLE;

typedef struct{
  pthread_cond_t *empty;
  pthread_cond_t *full;
  pthread_mutex_t *mutex;
  char **buf;
  int *pindex;
  int *index;
  int fd;
} DataProdWriter;

typedef struct {
  pthread_cond_t *empty;
  pthread_cond_t *full;
  pthread_mutex_t *mutex;
  char **buf;
  int *cindex;
  int *index;
} DataConsWriter;

void* tbody_prod_writer(void *args){
  DataProdWriter *data = (DataProdWriter*)args;
  int n;
  char *temp_buf;
  do {
    size_t e = readn(data->fd, &n, sizeof(n));
    if (e != sizeof(n))
      termina("Error in reading 1");

    temp_buf = malloc(n * sizeof(char));

    e = readn(data->fd, temp_buf, n);
    if (e != n)
      termina("Error in reading 2");

    // strncat(temp_buf, "0", 1);

    char *token = strtok(temp_buf, ".,:; \n\r\t");

    while (token != NULL) {
        xpthread_mutex_lock(data->mutex, QUI);
        while(*(data->index) == PC_BUFFER_LEN){
          xpthread_cond_wait(data->full, data->mutex, QUI);
        }
        data->buf[*(data->pindex) % PC_BUFFER_LEN] = token;
        *(data->index) += 1;
        *(data->pindex) += 1;
        token = strtok(NULL, ".,:; \n\r\t");
        xpthread_cond_signal(data->empty, QUI);
        xpthread_mutex_unlock(data->mutex, QUI);
    }

  } while (true); 
  free(temp_buf);
}

void* tbody_cons_writer(void* args){
  DataConsWriter *data = (DataConsWriter*)args;
  do {
    xpthread_mutex_lock(data->mutex, QUI);
    while (*(data->index) == 0) {
      xpthread_cond_wait(data->empty, data->mutex, QUI);
    }
    hash_table_insert(HASH_TABLE, data->buf[*(data->cindex) % PC_BUFFER_LEN]);
    *(data->cindex) += 1;
    *(data->index) -= 1;

    xpthread_cond_signal(data->full, QUI);
    xpthread_mutex_unlock(data->mutex, QUI);
  }while(true);
}

int main(int argc, char **argv){
  assert(argc == 3);
  int num_writers = atoi(argv[1]);
  int num_readers = atoi(argv[2]);

  HASH_TABLE = hash_table_create();

  int fd = open("caposc", O_RDONLY);
  int fd2 = open("capolet", O_RDONLY);

  char *buffer[PC_BUFFER_LEN];
  int pindex = 0, cindex = 0, index = 0;
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
  pthread_cond_t full = PTHREAD_COND_INITIALIZER;

  pthread_t prod_writer;
  DataProdWriter data_prod_writer;

  data_prod_writer.buf = &buffer[0];
  data_prod_writer.empty = &empty;
  data_prod_writer.full = &full;
  data_prod_writer.fd = fd;
  data_prod_writer.mutex = &mutex;
  data_prod_writer.pindex = &pindex;
  data_prod_writer.index = &index;

  pthread_t cons_writer[num_writers];
  DataConsWriter data_cons_writer[num_writers];

  xpthread_create(&prod_writer, NULL, &tbody_prod_writer, &data_prod_writer, QUI);

  for(int i=0; i<num_writers; i++){
    data_cons_writer[i].buf = &buffer[0];
    data_cons_writer[i].empty = &empty;
    data_cons_writer[i].full = &full;
    data_cons_writer[i].cindex = &cindex;
    data_cons_writer[i].index = &index;
    data_cons_writer[i].mutex = &mutex;

    xpthread_create(&cons_writer[i], NULL, &tbody_cons_writer, &data_cons_writer[i], QUI);
  }

  xpthread_join(prod_writer, NULL, QUI);

  for(int i=0; i<num_writers; i++){
    xpthread_join(cons_writer[i], NULL, QUI);
  }

  close(fd);
  close(fd2);
  
  hash_table_destroy(HASH_TABLE);
  return 0;
}