#include "actor.h"

int actor_init(actor_t* actor, thread_pool_t* pool, role_t *role, actor_system_t* a_system) {
  if (actor == NULL) {
    fprintf(stderr, "malloc of actor_t\n");
    return -1;
  }

  if (pthread_mutex_init(&(actor->lock), NULL) != 0) {
    fprintf(stderr, "pthread_mutex_init actor->lock\n");
    return -1;
  }

  if (pthread_mutex_init(&(actor->dead_lock), NULL) != 0) {
    fprintf(stderr, "pthread_mutex_init actor->dead_lock\n");
    return -1;
  }

  actor->message_queue = new_queue(1024, ACTOR_QUEUE_LIMIT);
  if (actor->message_queue == NULL) {
    fprintf(stderr, "new_queue\n");
    return -1;
  }

  actor->pool = pool;
  actor->role = role;
  actor->a_system = a_system;
  actor->scheduled = false;
  actor->dead = false;
  actor->state = NULL;

  return 0;
}

void actor_destroy(actor_t* actor) {
  if (pthread_mutex_destroy(&(actor->lock)) != 0) {
    syserr("pthread_mutex_destoy");
  }

  if (pthread_mutex_destroy(&(actor->dead_lock)) != 0) {
    syserr("pthread_mutex_destoy");
  }

  while (actor->message_queue->length > 0) {
    message_t* m = queue_pop(actor->message_queue);
    free(m);
  }

  queue_destroy(actor->message_queue);
  free(actor);
}

// push message to queue and schedule actor
int actor_push_message(actor_t* actor, message_t* message) {
  if (pthread_mutex_lock(&(actor->lock)) != 0) {
    syserr("mutex lock 4");
  }

  if (queue_push(actor->message_queue, message) != 0) {
    free(message);
    if (pthread_mutex_unlock(&(actor->lock)) != 0) {
      syserr("mutex unlock");
    }
    return -1;
  }

  if (!actor->scheduled) {
    actor->scheduled = true;

    runnable_t runnable;
    runnable.arg = actor;
    runnable.argsz = sizeof(actor_t*);
    runnable.function = (void (*)(void *, size_t))&actor_process_message;
    defer(actor->pool, runnable);
  }

  if (pthread_mutex_unlock(&(actor->lock))) {
    syserr("mutex unlock");
  }
  return 0;
}

void handle_spawn(actor_t* actor, message_t* message) {
  actor_t* new_actor = malloc(sizeof(actor_t));
  if (new_actor == NULL) {
    return;
  }
  if (actor_init(new_actor, actor->pool, message->data, actor->a_system) != 0) {
    free(new_actor);
    return;
  }

  if (actor_system_insert(actor->a_system, new_actor) == -1) {
    free(new_actor);
    return;
  }

  message_t hello_message = {
    .message_type = MSG_HELLO,
    .data = (actor_id_t*)&actor->id,
    .nbytes = sizeof(actor_id_t*)
  };
  send_message(new_actor->id, hello_message);
}

void handle_godie(actor_t* actor, __attribute__((unused)) message_t* message) {
  actor_set_dead(actor, true);

  if (pthread_mutex_lock(&(actor->a_system->lock)) != 0) {
    syserr("pthread_mutex_lock");
    return;
  }

  actor->a_system->alive--;

  if (actor->a_system->alive == 0) {
    if (pthread_cond_signal(&(actor->a_system->finished_cond)) != 0) {
      syserr("pthread_cond_signal");
      return;
    }
  }

  if (pthread_mutex_unlock(&(actor->a_system->lock)) != 0) {
    syserr("pthread_mutex_unlock");
    return;
  }

}

void handle_hello(actor_t* actor, message_t* message) {
  actor->role->prompts[0](&(actor->state), message->nbytes, message->data);
}

// pulls and executes one message from message queue for scheduled actor
void actor_process_message(actor_t* actor, __attribute__((unused)) size_t argsz) {

  if (pthread_mutex_lock(&(actor->lock)) != 0) {
    syserr("mutex lock 3");
    return;
  }

  actor_id_set(actor->id);

  if (!actor->scheduled) {
    syserr("actor is currently not scheduled");
    return;
  }
  message_t* message = (message_t*)queue_pop(actor->message_queue);

  if (pthread_mutex_unlock(&(actor->lock)) != 0) {
    syserr("mutex unlock");
    return;
  }

  if(actor_is_dead(actor)) {
    fprintf(stderr,"actor_process_image: actor %lu is already dead, skipping\n", actor->id);
    free(message);
    return;
  }

  switch (message->message_type) {
    case MSG_SPAWN:
      handle_spawn(actor, message);
      break;
    case MSG_GODIE:
      handle_godie(actor, message);
      free(message);
      return;
    case MSG_HELLO:
      handle_hello(actor, message);
      break;
    default:

      if (message->message_type < 0 || (size_t)message->message_type >= actor->role->nprompts) {
        syserr("Wrong message type");
        return;
      }

      actor->role->prompts[message->message_type](&(actor->state), message->nbytes, message->data);
  }
  free(message);

  if (pthread_mutex_lock(&(actor->lock)) != 0) {
    syserr("mutex lock 2");
    return;
  }

  if (actor->message_queue->length == 0) {
    actor->scheduled = false;
  }
  else {
    runnable_t runnable;
    runnable.arg = actor;
    runnable.argsz = sizeof(actor_t*);
    runnable.function = (void (*)(void *, size_t))&actor_process_message;
    defer(actor->pool, runnable);
  }

  if (pthread_mutex_unlock(&(actor->lock)) != 0) {
    syserr("mutex unlock");
    return;
  }
}

bool actor_is_dead(actor_t* actor) {
  if (pthread_mutex_lock(&(actor->dead_lock)) != 0) {
    syserr("mutex lock  2");
    return true;
  }
  bool ret = actor->dead;
  if (pthread_mutex_unlock(&(actor->dead_lock)) != 0) {
    syserr("mutex unlock");
    return true;
  }
  return ret;
}

void actor_set_dead(actor_t* actor, bool value) {
  if (pthread_mutex_lock(&(actor->dead_lock)) != 0) {
    syserr("mutex lock 5");
  }
  if (value && actor->dead) {
    syserr("cannot kill actor twice");
  }
  actor->dead = value;
  if (pthread_mutex_unlock(&(actor->dead_lock)) != 0) {
    syserr("mutex unlock 6");
  }
}

