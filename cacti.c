#include "cacti.h"


actor_system_t* a_system = NULL;

int actor_system_create(actor_id_t *actor, role_t *const role) {
  a_system = (actor_system_t*)malloc(sizeof(actor_system_t));
  if (a_system == NULL) {
    fprintf(stderr, "malloc actor_system_t\n");
    return -1;
  }

  if (actor_system_init(a_system) != 0) {
    fprintf(stderr, "actor_system_init\n");
    return -1;
  }

  actor_t* first_actor = malloc(sizeof(actor_t));
  if (first_actor == NULL) {
    fprintf(stderr, "malloc\n");
    return -1;
  }

  if (actor_init(first_actor, a_system->pool, role, a_system) != 0) {
    fprintf(stderr, "actor_init\n");
    return -1;
  }

  *actor = actor_system_insert(a_system, first_actor);
  if (*actor < 0) {
    fprintf(stderr, "actor_system_insert\n");
    return -1;
  }

  return 0;
}

void actor_system_join(actor_id_t actor) {
  // todo
}

int send_message(actor_id_t actor_id, message_t message) {
  actor_t* actor = actor_system_find(a_system, actor_id);

  printf("send_message: actor_id = %ld, message->message_type = %ld\n", actor_id, message.message_type);
  
  // no actor with that id in system
  if (actor == NULL) {
    return -2; 
  }

  if (actor_push_message(actor, &message) != 0) {
    return -1;
  }
  printf("send_message: sent!\n");
  return 0;
}


