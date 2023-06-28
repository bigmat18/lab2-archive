#include "utils.h"

#define QUI __LINE__, __FILE__
#define NUM_ELEM 1000000
#define PC_BUFFER_LEN 10

int HASH_TABLE;

void aggiungi(char *s);
int conta(char *s);
ENTRY *create_entry(char *s, int n);
void remove_entry(ENTRY *e);

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

void tbody_prod_writer(void *args){
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

    strncat(temp_buf, "0", 1);

    char *token = strtok(temp_buf, ".,:; \n\r\t");

    while (token != NULL) {
        xpthread_mutex_lock(data->mutex, QUI);
        while(*(data->index) == PC_BUFFER_LEN){
          xpthread_cond_wait(data->full, data->mutex, QUI);
        }
        data->buf[*(data->pindex) % PC_BUFFER_LEN] = strdup(token);
        *(data->index) += 1;
        *(data->pindex) += 1;
        token = strtok(NULL, ".,:; \n\r\t");
        xpthread_cond_signal(data->empty, QUI);
        xpthread_mutex_unlock(data->mutex, QUI);
    }

  } while (true); 
  free(temp_buf);
}

void tbody_cons_writer(void* args){
  DataConsWriter *data = (DataConsWriter*)args;
  do {
    xpthread_mutex_lock(data->mutex, QUI);
    while (*(data->index) == 0) {
      xpthread_cond_wait(data->empty, data->mutex, QUI);
    }
    aggiungi(data->buf[*(data->cindex) % PC_BUFFER_LEN]);
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

  HASH_TABLE = hcreate(NUM_ELEM);
  if (HASH_TABLE == 0)
    termina("Error creation hash table");
  int fd = open("caposc", O_RDONLY);

  char *buffer[PC_BUFFER_LEN];
  int pindex = 0, cindex = 0, index = 0;
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
  pthread_cond_t full = PTHREAD_COND_INITIALIZER;

  pthread_t prod_writer;
  DataProdWriter data;

  data.buf = &buffer[0];
  data.empty = &empty;
  data.full = &full;
  data.fd = fd;
  data.mutex = &mutex;
  data.pindex = &pindex;
  data.index = &index;

  xpthread_create(&prod_writer, NULL, &tbody_prod_writer, &data, QUI);
  xpthread_join(prod_writer, NULL, QUI);

  for(int i=0; i<PC_BUFFER_LEN; i++) printf("%s\n", buffer[i]);

  close(fd);
  hdestroy();
  return 0;
}

ENTRY *crea_entry(char *s, int n) {
  ENTRY *e = malloc(sizeof(ENTRY));
  if(e==NULL) termina("errore malloc entry 1");

  e->key = strdup(s);
  e->data = (int *) malloc(sizeof(int));

  if(e->key==NULL || e->data==NULL)
    termina("errore malloc entry 2");
  *((int *)e->data) = n;
  return e;
}

void remove_entry(ENTRY *e){
  free(e->key);
  free(e->data);
  free(e);
}

void aggiungi(char *s){
  ENTRY *value = hsearch((ENTRY){s, (int)(1)}, FIND);
  if (value  != NULL){
    (int)(value->data)++;
  } else {
    hsearch((ENTRY){s, (int)(1)}, ENTER);
  }
}