#include<stdlib.h>
#include<math.h>
#include "my_malloc.h"

int total_bytes_allocated = 0;

void *my_malloc(int size) {
  void *p;

  total_bytes_allocated += size;
  p = malloc(size + sizeof(int));  // assuming that malloc returns word aligned pointers
  ((int *)p)[0] = size;
  p += sizeof(int);
  return(p);
}

void my_free(void *p) {
  p -= sizeof(int);
  int size = ((int *)p)[0];
  total_bytes_allocated -= size;
  free(p);
}
