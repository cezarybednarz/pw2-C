#include "cacti.h"

actor_system_t* a_system;

int actor_system_create(actor_id_t *actor, role_t *const role) {
  if (actor_system_init(a_system) != -1) {
    return -1;
  }

  actor_t* first_actor = malloc(sizeof(actor_t));
  if (first_actor == NULL) {
    return -1;
  }

  if (actor_init(first_actor, a_system->pool, role, a_system) != 0) {
    return -1;
  }

  if (actor_system_insert(a_system, first_actor) != 0) {
    return -1;
  }

  *actor = 0;

  return 0;
}

void actor_system_join(actor_id_t actor) {
  // todo
}

int send_message(actor_id_t actor, message_t message) {
  // todo
}


