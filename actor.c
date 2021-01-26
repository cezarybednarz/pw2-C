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

  actor->message_queue = new_queue();
  if (actor->message_queue == NULL) {
    fprintf(stderr, "new_queue\n");
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
  pthread_mutex_destroy(&(actor->lock));
  queue_destroy(actor->message_queue);
  free(actor);
}

// push message to queue and schedule actor
int actor_push_message(actor_t* actor, message_t* message) {
  if (pthread_mutex_lock(&(actor->lock)) != 0) {
    return -1;
  }

  // todo handle actor death

  if (queue_push(actor->message_queue, message) != 0) {
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
    return -1;
  }
  return 0;
}

void handle_spawn(actor_t* actor, message_t* message) {

  actor_t* new_actor = malloc(sizeof(actor_t));
  if (new_actor == NULL) {
    return;
  }
  if (actor_init(new_actor, actor->pool, message->data, actor->a_system) != 0) {
    return;
  }

  actor_system_insert(actor->a_system, new_actor);

  message_t hello_message;
  hello_message.message_type = MSG_HELLO;
  hello_message.data = (actor_id_t*)&actor->id;
  hello_message.nbytes = sizeof(actor_id_t);
  if (send_message(new_actor->id, hello_message) != 0) {
    return;
  }
}

void handle_godie( __attribute__ ((unused)) actor_t* actor,  __attribute__ ((unused)) message_t* message) {
  printf("MSG_GODIE not handled yet");
  
  // todo
}

void handle_hello(actor_t* actor, message_t* message) {
  actor_id_t parent_actor = (actor_id_t)message->data;
  printf("actor %d received MSG_HELLO from actor %d\n", (int)actor->id, (int)parent_actor);
  // todo zweryfikowac
  //actor->role->prompts[0](NULL, 0, NULL);
}

// pulls and executes one message from message queue for scheduled actor
void actor_process_message(actor_t* actor, size_t argsz) {

  printf("starting processing message for actor %d\n", (int)actor->id);

  if (pthread_mutex_lock(&(actor->lock)) != 0) {
    return;
  }

  if (!actor->scheduled) {
    syserr("actor is currently not scheduled");
    return;
  }

  message_t* message = (message_t*)queue_pop(actor->message_queue);

  printf("popped message: %d\n", (int)message->message_type);

  if (pthread_mutex_unlock(&(actor->lock))) {
    return;
  }

  switch (message->message_type) {
    case MSG_SPAWN:
      handle_spawn(actor, message);
      break;
    case MSG_GODIE:
      handle_godie(actor, message);
      break;
    case MSG_HELLO:
      handle_hello(actor, message);
      break;
    default:

      if (message->message_type < 0 || (size_t)message->message_type >= actor->role->nprompts) {
        syserr("Wrong message type");
        return;
      }  

      // todo
      printf("Other messages are currently not handled properly\n");
  }


  return;
}

