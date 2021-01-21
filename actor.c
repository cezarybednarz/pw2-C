#include "actor.h"

int actor_init(actor_t* actor, thread_pool_t* pool, role_t *role, actor_system_t* a_system) {
  actor = (actor_t*)malloc(sizeof(actor));
  if (actor == NULL) {
    return -1;
  }

  if (pthread_mutex_init(actor->lock, NULL) != 0) {
    return -1;
  }

  actor->message_queue = new_queue();
  if (actor->message_queue == NULL) {
    return -1;
  }

  actor->pool = pool;
  actor->role = role;
  actor->a_system = a_system;
  actor->scheduled = false;
  actor->dead = false;

  return 0;
}

void actor_destroy(actor_t* actor) {
  pthread_mutex_destroy(actor->lock);
  queue_destroy(actor->message_queue);
  free(actor);
}

// push message to queue and schedule actor
int actor_push_message(actor_t* actor, message_t* message) {
  if (pthread_mutex_lock(actor->lock) != 0) {
    return -1;
  }
  
  if (queue_push(actor->message_queue, message) != 0) {
    return -1;
  }

  if (!actor->scheduled) {
    actor->scheduled = true;

    runnable_t runnable;
    runnable.arg = actor;
    runnable.argsz = sizeof(actor_t*);
    runnable.function = actor_process_message;
    defer(actor->pool, runnable);
  }

  if (pthread_mutex_unlock(actor->lock)) {
    return -1;
  }
}

// pulls and executes one message from message queue for scheduled actor
int actor_process_message(actor_t* actor) {

  if (pthread_mutex_lock(actor->lock) != 0) {
    return -1;
  }

  if (!actor->scheduled) {
    syserr("actor is currently not scheduled");
    return -1;
  }

  message_t* message = (message_t*)queue_pull(actor->message_queue, message);

  if (pthread_mutex_unlock(actor->lock)) {
    return -1;
  }

  switch (message->message_type) {
    case MSG_SPAWN:
      actor_t* new_actor = malloc(sizeof(actor_t));
      if (new_actor == NULL) {
        return -1;
      }
      if (actor_init(new_actor, actor->pool, message->data, actor->a_system) != 0) {
        return -1;
      }

      actor_system_insert(actor->a_system, new_actor);

      message_t* hello_message;
      hello_message->message_type = MSG_HELLO;
      hello_message->data = actor->id;
      hello_message->nbytes = sizeof(actor_id_t);
      if (actor_push_message(new_actor, hello_message) != 0) {
        return -1;
      }

      break;
    case MSG_GODIE:
      // todo
      break;  
    case MSG_HELLO:
      actor_id_t parent_actor = (actor_id_t)message->data;

      printf("actor %d received MSG_HELLO from actor %d\n", actor->id, parent_actor);
      
      // todo zweryfikowac
      actor->role->prompts[0](NULL, 0, NULL);

      break;
    default:
      if (message->message_type >= actor->role->nprompts) {
        syserr("Wrong message type");
        return -1;
      }  
      // todo
  }


  return 0;
}

