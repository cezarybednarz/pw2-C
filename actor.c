#include "actor.h"

int actor_init(actor_t* actor, thread_pool_t* pool, role_t *role) {
  actor = (actor_t*)malloc(sizeof(actor));
  if (actor == NULL) {
    return -1;
  }

  if (pthread_mutex_init(actor->queue_lock, NULL) != 0) {
    return -1;
  }

  if (pthread_mutex_init(actor->currently_working_lock, NULL) != 0) {
    return -1;
  }

  actor->message_queue = new_queue();
  if (actor->message_queue == NULL) {
    return -1;
  }

  actor->pool = pool;
  actor->role = role;

  return 0;
}

void actor_destroy(actor_t* actor) {
  pthread_mutex_destroy(actor->queue_lock);
  queue_destroy(actor->message_queue);
  free(actor);
}

typedef struct handle_arg {
  actor_t* actor;
  message_t* message;
} handle_arg_t;

void actor_handle_message(handle_arg_t arg, size_t argsz) {
  message_t* message = arg.message;
  actor_t* actor = arg.actor;
  

  free(&arg);
}

int actor_receive_message(actor_t* actor, message_t* message) {
  // push message
  if (pthread_mutex_lock(actor->queue_lock) != 0) {
    return -1;
  }
  if (queue_push(actor->message_queue, message) != 0) {
    pthread_mutex_unlock(actor->queue_lock);
    return -1;
  }
  if (pthread_mutex_unlock(actor->queue_lock)) {
    return -1;
  }
  
  // pull message to work on
  message_t* new_message;
  if (pthread_mutex_lock(actor->queue_lock) != 0) {
    return -1;
  }
  message = queue_pop(actor->message_queue);
  if (pthread_mutex_unlock(actor->queue_lock)) {
    return -1;
  }

  // work on message
  if (pthread_mutex_lock(actor->currently_working_lock) != 0) {
    return -1;
  }

  // prepare function to run on thread pool
  
  handle_arg_t* handle_arg = (handle_arg_t*)malloc(sizeof(handle_arg_t));
  if (handle_arg == NULL) {
    return -1;
  }
  handle_arg->actor = actor;
  handle_arg->message = message;
  runnable_t runnable;
  runnable.function = actor_handle_message;
  runnable.arg = handle_arg;
  runnable.argsz = sizeof(handle_arg_t);

  if (defer(actor->pool, runnable) != 0) {
    pthread_mutex_unlock(actor->currently_working_lock);
    return -1;
  }

  if (pthread_mutex_unlock(actor->currently_working_lock) != 0) {
    return -1;
  }

  return 0;
}

