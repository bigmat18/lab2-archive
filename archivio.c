#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <search.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#define NUM_ELEM 1000000
#define PC_BUFFER_LEN 10

typedef struct {
    int data;
    ENTRY *next;
} Data;

void terminate(const char *message){
  if(errno!=0) perror(message);
	else fprintf(stderr,"%s\n", message);
  exit(1);
}

ENTRY *create_entry(char *s, int n) {
  ENTRY *e = malloc(sizeof(ENTRY));

  if(e==NULL) terminate("error malloc entry 1");
  e->key = strdup(s);
  e->data = (Data*)malloc(sizeof(Data));

  if(e->key==NULL || e->data==NULL)
    terminate("error malloc entry 2");

  ((Data*)e->data)->data = n;
  ((Data*)e->data)->next = NULL;
  return e;
}

void remove_entry(ENTRY *e) {
  free(e->key); 
  free(e->data); 
  free(e);
}

void aggiungi(char *s){

}

int conta(char *s){

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

int main(int argc, char **argv){
    // assert(argc == 3);
    int num_writers = atoi(argv[1]);
    int num_readers = atoi(argv[2]);

    char buf[3];
    int hash_table = hcreate(NUM_ELEM);
    if(hash_table == 0) terminate("Error creation hash table");
    printf("ciao");
    int fd = open("caposc", O_RDONLY);
    int bytesread = 0;

    while (1) {
      char temp;
      if (read(fd, &temp, 1) < 1) break;
      fprintf(stderr, "chaine : %c\n", temp);
    }

    close(fd);
    return 0;
}
