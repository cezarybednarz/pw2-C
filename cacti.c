#include "cacti.h"

_Thread_local actor_id_t actor_id_thread_local;

void actor_id_set(actor_id_t id) {
  actor_id_thread_local = id;
}

actor_id_t actor_id_self() {
  return actor_id_thread_local;
}

actor_system_t a_system;

int actor_system_create(actor_id_t *actor, role_t *const role) {

  if (actor_system_init(&a_system) != 0) {
    fprintf(stderr, "actor_system_init\n");
    return -1;
  }

  actor_t* first_actor = malloc(sizeof(actor_t));
  if (first_actor == NULL) {
    fprintf(stderr, "malloc\n");
    return -1;
  }

  if (actor_init(first_actor, a_system.pool, role, &a_system) != 0) {
    fprintf(stderr, "actor_init\n");
    return -1;
  }

  *actor = actor_system_insert(&a_system, first_actor);
  if (*actor < 0) {
    fprintf(stderr, "actor_system_insert\n");
    return -1;
  }

  // forum: implicit MSG_HELLO sent after actor_system_create()
  message_t msg_hello = {
    .message_type = MSG_HELLO,
    .data = (void*)actor_id_self()
  };
  send_message(*actor,  msg_hello);

  return 0;
}

void actor_system_join(actor_id_t actor) {

  if (actor_system_find(&a_system, actor) == NULL) {
    return;
  }

  if (pthread_mutex_lock(&(a_system.lock)) != 0) {
    syserr("pthread_mutex_lock");
    return;
  }
  while (a_system.alive > 0) {
    if (pthread_cond_wait(&(a_system.finished_cond), &(a_system.lock)) != 0) {
      syserr("pthread_cond_wait");
      return;
    }
  }
  if (pthread_mutex_unlock(&(a_system.lock)) != 0) {
    syserr("pthread_mutex_unlock");
    return;
  }

  actor_system_destroy(&a_system);
}

int send_message(actor_id_t actor_id, message_t message) {
  actor_t* actor = actor_system_find(&a_system, actor_id);

  // no actor with that id in system
  if (actor == NULL) {
    return -2; 
  }


  if (actor_is_dead(actor)) {
    return -1;
  }

  message_t* message_copy = malloc(sizeof(message_t));
  message_copy->message_type = message.message_type;
  message_copy->data = message.data;
  message_copy->nbytes = message.nbytes;

  if (actor_push_message(actor, message_copy) != 0) {
    free(message_copy);
    fprintf(stderr, "send_message: error while pushing message, queue full\n");
    return -3;
  }

  return 0;
}
