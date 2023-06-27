#include "utils.h"

#define NUM_ELEM 1000000
#define PC_BUFFER_LEN 10

void aggiungi(char *s);
int conta(char *s);
ENTRY *create_entry(char *s, int n);
void remove_entry(ENTRY *e);

typedef struct {
    int data;
    ENTRY *next;
} Data;

typedef struct {
    pthread_cond_t *empty;
    pthread_cond_t *full;
    pthread_mutex_t *mutex_buf;
    char *buf;
    int *fd;
} DataChiefConsumer;

int main(int argc, char **argv){
    assert(argc == 3);
    int num_writers = atoi(argv[1]);
    int num_readers = atoi(argv[2]);

    int hash_table = hcreate(NUM_ELEM);
    if(hash_table == 0) termina("Error creation hash table");
    int fd = open("caposc", O_RDONLY);
    int n;

    char *buf;

    while(true){
      size_t e = readn(fd, &n, sizeof(n));
      if (e != sizeof(n))
          termina("Error in reading 1");
      printf("%d - ", n);

      buf = malloc(n * sizeof(char));

      e = readn(fd, buf, n); 
      if (e != n)
          termina("Error in reading 2");

      printf("%s\n", buf);
    }

    free(buf);

    close(fd);
    return 0;
}

ENTRY *create_entry(char *s, int n) {
  ENTRY *e = malloc(sizeof(ENTRY));

  if(e==NULL) termina("error malloc entry 1");
  e->key = strdup(s);
  e->data = (Data*)malloc(sizeof(Data));

  if(e->key==NULL || e->data==NULL)
    termina("error malloc entry 2");

  ((Data*)e->data)->data = n;
  ((Data*)e->data)->next = NULL;
  return e;
}

void remove_entry(ENTRY *e) { 
  free(e->key); 
  free(e->data); 
  free(e); 
}

