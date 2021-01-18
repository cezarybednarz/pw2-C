#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <semaphore.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>

#include <stdarg.h>
#include <string.h>
#include "err.h"
#include "queue.h"

/* ---- signal handling ---- */
pthread_mutex_t pools_queue_mutex;
struct queue *pools_queue;

/* ---- threadpool ---- */
typedef struct runnable {
  void (*function)(void *, size_t);
  void*  arg;
  size_t argsz;
} runnable_t;

typedef struct thread_pool {
  size_t num_threads;
  size_t num_threads_started;

  pthread_t*     threads;
  struct queue*  defer_queue;
  bool           destroyed;

  pthread_mutex_t  mutex;
  pthread_cond_t   condition;

} thread_pool_t;

int thread_pool_init(thread_pool_t *pool, size_t pool_size);

void thread_pool_destroy(thread_pool_t *pool);

int defer(thread_pool_t *pool, runnable_t runnable);

#endif // THREADPOOL_H
