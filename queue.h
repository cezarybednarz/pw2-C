#ifndef DEFER_QUEUE_H
#define DEFER_QUEUE_H

#include <pthread.h>
#include <semaphore.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <stdarg.h>
#include <string.h>
#include "err.h"
#include "threadpool.h"

typedef struct node node_t;

typedef struct node {
  void*        data;
  node_t*      prev;
} node_t;

typedef struct queue {
  node_t*         front;
  node_t*         back;
  int             length;
} queue_t;

queue_t *new_queue(void);

int queue_push(queue_t *q, void *data);

void *queue_pop(queue_t *q);

void queue_destroy(queue_t *q);



#endif //DEFER_QUEUE_H
