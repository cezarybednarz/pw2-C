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

int actor_handle_message(actor_t* actor, message_t* message) {

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
  message_t* message;
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

  if (MSG_SPAWN == message->message_type) {
    
  } else if (MSG_GODIE == message->message_type) {

  } else if (MSG_HELLO == message->message_type) {

  } else {
    printf("Message type unsupported: %d\n", message->message_type);
  }



  runnable_t* runnable = (runnable_t*)malloc(sizeof(runnable_t));




  if (pthread_mutex_unlock(actor->currently_working_lock) != 0) {
    return -1;
  }

  return 0;
}

