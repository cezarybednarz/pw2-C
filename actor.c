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

  printf("actor_push_message: starting\n");
  if (pthread_mutex_lock(&(actor->lock)) != 0) {
    return -1;
  }

  printf("actor_push_message: after mutex\n");
  printf("actor_push_message: actor->id = %ld, message->message_type = %ld\n", actor->id, message->message_type);

  // todo handle actor death

  if (queue_push(actor->message_queue, message) != 0) {
    return -1;
  }

  printf("actor_push_message: messages on queue for actor %ld:\n", actor->id);
  for (size_t i = 0; i < actor->message_queue->front; i++) {
    message_t* m = (message_t*)actor->message_queue->data_array[i];
    printf("[%ld] ", m->message_type);
  }
  printf("\n");

  if (!actor->scheduled) {
    printf("actor_push_message: actor is unscheduled!\n");
    actor->scheduled = true;

    runnable_t runnable;
    runnable.arg = actor;
    runnable.argsz = sizeof(actor_t*);
    runnable.function = (void (*)(void *, size_t))&actor_process_message;
    defer(actor->pool, runnable);
  }

  printf("actor_push_message: actor = %lu\n", (size_t)actor);
  printf("actor_push_message: actor->message_queue = %lu\n", (size_t)actor->message_queue);
  printf("actor_push_message: messages on queue for actor %ld (after defer):\n", actor->id);
  for (size_t i = 0; i < actor->message_queue->front; i++) {
    message_t* m = (message_t*)actor->message_queue->data_array[i];
    printf("[%ld] ", m->message_type);
  }
  printf("\n");

  printf("actor_push_message: mutex unlock\n");
  if (pthread_mutex_unlock(&(actor->lock))) {
    return -1;
  }
  return 0;
}

void handle_spawn(actor_t* actor, message_t message) {

  printf("handle_spawn: starting\n");

  actor_t* new_actor = malloc(sizeof(actor_t));
  if (new_actor == NULL) {
    return;
  }
  if (actor_init(new_actor, actor->pool, message.data, actor->a_system) != 0) {
    return;
  }

  actor_system_insert(actor->a_system, new_actor);

  message_t* hello_message = (message_t*)malloc(sizeof(message_t));
  if (hello_message == NULL) {
    return;
  }
  hello_message->message_type = MSG_HELLO;
  hello_message->data = (actor_id_t*)&actor->id;
  hello_message->nbytes = sizeof(actor_id_t);
  if (actor_push_message(new_actor, hello_message) != 0) {
    return;
  }
}

void handle_godie( __attribute__ ((unused)) actor_t* actor,  __attribute__ ((unused)) message_t message) {
  printf("handle_godie: MSG_GODIE not handled yet");
  
  // todo
}

void handle_hello(actor_t* actor, message_t message) {
  actor_id_t* parent_actor = (actor_id_t*)message.data;
  printf("handle_hello: actor %ld received MSG_HELLO from actor %ld\n", actor->id, *parent_actor);
  // todo zweryfikowac
  //actor->role->prompts[0](NULL, 0, NULL);
}

// pulls and executes one message from message queue for scheduled actor
void actor_process_message(actor_t* actor, size_t argsz) {

  if (pthread_mutex_lock(&(actor->lock)) != 0) {
    return;
  }

  printf("actor_process_message: actor = %lu\n", (size_t)actor);
  printf("actor_process_message: actor->message_queue = %lu\n", (size_t)actor->message_queue);
  printf("actor_process_message: after lock\n");

  printf("actor_process_message: messages on queue for actor %ld:\n", actor->id);
  for (size_t i = 0; i < actor->message_queue->front; i++) {
    message_t* m = (message_t*)actor->message_queue->data_array[i];
    printf("[%ld] ", m->message_type);
  }
  printf("\n");

  printf("actor_process_message: starting processing message for actor %d\n", (int)actor->id);
  printf("actor_process_message: after mutex\n");

  if (!actor->scheduled) {
    syserr("actor is currently not scheduled");
    return;
  }

  printf("actor_process_message: actor->message_queue->length %lu\n", actor->message_queue->length);

  message_t message = *((message_t*)queue_pop(actor->message_queue));

  printf("actor_process_message: actor_id = %ld, message_type = %ld\n", actor->id, message.message_type);
  printf("actor_process_message: unlocking mutex\n");

  if (pthread_mutex_unlock(&(actor->lock))) {
    return;
  }

  switch (message.message_type) {
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

      if (message.message_type < 0 || (size_t)message.message_type >= actor->role->nprompts) {
        syserr("Wrong message type");
        return;
      }  

      // todo
      printf("Other messages are currently not handled properly\n");
  }

  return;
}

