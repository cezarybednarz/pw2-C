#include "threadpool.h"

#define ERR -1

/* ---- signal handling ---- */
pthread_mutex_t pools_queue_mutex;
struct queue *pools_queue;

/* ---- sigaction handling ---- */
void __attribute__((destructor)) destroy_signals() {
  pthread_mutex_destroy(&pools_queue_mutex);
  queue_destroy(pools_queue);
}

static void termination_handler(int sig_id __attribute__((unused))) {
  exit(0);
}

void __attribute__((constructor)) init_signals() {
  struct sigaction new_action, old_action;
  new_action.sa_handler = termination_handler;
  sigemptyset(&new_action.sa_mask);
  sigaddset(&new_action.sa_mask, SIGTERM);
  new_action.sa_flags = 0;
  sigaction(SIGINT, NULL, &old_action);
  if(old_action.sa_handler != SIG_IGN) {
    sigaction(SIGINT, &new_action, NULL);
  }

  pthread_mutex_init(&pools_queue_mutex, NULL);
  pools_queue = new_queue(16, 0);
}



/* ---- single thread waiting loop ---- */
static void *thread_loop(void *pool) {

  thread_pool_t *p = (thread_pool_t*)pool;
  runnable_t *todo;

  while (true) {
    pthread_mutex_lock(&(p->mutex));

    while (!p->destroyed && p->defer_queue->length == 0) {
      pthread_cond_wait(&(p->condition), &(p->mutex));
    }

    if (p->destroyed && p->defer_queue->length == 0) {
      break;
    }

    todo = (runnable_t*)queue_pop(p->defer_queue);
    pthread_mutex_unlock(&(p->mutex));
    todo->function(todo->arg, todo->argsz);
    free(todo);
  }

  p->num_threads_started--;
  pthread_mutex_unlock(&(p->mutex));
  return (NULL);
}

/* ---- thread pool ---- */
int thread_pool_init(thread_pool_t *pool, size_t num_threads) {

  if (pool == NULL) {
    return ERR;
  }


  /* initialising global list of thread_pools and changing default behaviour to SIGINT */
  if (pthread_mutex_lock(&pools_queue_mutex) != 0) {
    return ERR;
  }

  queue_push(pools_queue, (void*)pool);

  if (pthread_mutex_unlock(&pools_queue_mutex) != 0) {
    return ERR;
  }

  pool->num_threads_started = 0;
  pool->num_threads = 0;
  pool->destroyed = false;

  if ((pool->threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t))) == NULL) {
    return ERR;
  }

  if ((pool->defer_queue = new_queue(16, 0)) == NULL) {
    free(pool->threads);
    return ERR;
  }

  if (pthread_mutex_init(&(pool->mutex), NULL) != 0) {
    free(pool->threads);
    queue_destroy(pool->defer_queue);
    return ERR;
  }

  if (pthread_cond_init(&(pool->condition), NULL) != 0) {
    free(pool->threads);
    queue_destroy(pool->defer_queue);
    if(pthread_mutex_destroy(&(pool->mutex)) != 0) {
      return ERR;
    }
    return ERR;
  }

  for (size_t i = 0; i < num_threads; i++) {
    if (pthread_create(&(pool->threads[i]), NULL, thread_loop, (void*)pool) != 0) {
      thread_pool_destroy(pool);
      return ERR;
    }
    pool->num_threads_started++;
    pool->num_threads++;
  }
  return 0;
}

void thread_pool_destroy(struct thread_pool *pool) {
  if (pool == NULL) {
    exit(ERR);
  }

  if (pthread_mutex_lock(&(pool->mutex)) != 0) {
    exit(ERR);
  }

  if (pool->destroyed) {
    return;
  }

  pool->destroyed = true;

  if (pthread_cond_broadcast(&(pool->condition)) != 0 ||
     pthread_mutex_unlock(&(pool->mutex)) != 0) {
    exit(ERR);
  }

  for (size_t i = 0; i < pool->num_threads; i++) {
    if(pthread_join(pool->threads[i], NULL) != 0) {
      exit(ERR);
    }
  }

  if (pool->num_threads_started > 0) {
    exit(ERR);
  }

  free(pool->threads);
  queue_destroy(pool->defer_queue);

  pthread_mutex_lock(&(pool->mutex));
  pthread_mutex_destroy(&(pool->mutex));
  pthread_cond_destroy(&(pool->condition));
}

int defer(struct thread_pool *pool, runnable_t runnable) {

  if (pool == NULL) {
    return ERR;
  }

  if (pthread_mutex_lock(&(pool->mutex)) != 0) {
    return ERR;
  }
  if (pool->destroyed) {
    if(pthread_mutex_unlock(&(pool->mutex)) != 0) {
      return ERR;
    }
  }
  else {

    runnable_t *new_runnable = malloc(sizeof(runnable));
    if(new_runnable == NULL) {
      return ERR;
    }
    new_runnable->function = runnable.function;
    new_runnable->arg = runnable.arg;
    new_runnable->argsz = runnable.argsz;
    if(queue_push(pool->defer_queue, (void*)new_runnable) != 0) {
      return ERR;
    }
    if(pthread_cond_signal(&(pool->condition)) != 0) {
      if(pthread_mutex_unlock(&(pool->mutex)) != 0) {
        return ERR;
      }
      return ERR;
    }
    if(pthread_mutex_unlock(&(pool->mutex)) != 0) {
      return ERR;
    }
  }

  return 0;
}
