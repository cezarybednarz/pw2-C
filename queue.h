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

typedef struct queue {
  size_t   front;
  size_t   back;
  void**   data_array;

  size_t   length;
  size_t   capacity;
  size_t   max_length;
} queue_t;

queue_t *new_queue(size_t base_capacity, size_t max_length);

int queue_push(queue_t *q, void *data);

void *queue_pop(queue_t *q);

void queue_destroy(queue_t *q);


#endif //DEFER_QUEUE_H
