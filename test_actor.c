#include "cacti.h"

int main() {
  thread_pool_t* pool = malloc(sizeof(thread_pool_t));
  thread_pool_init(pool, 1);

  role_t* role = NULL;

  actor_system_t* a_system = malloc(sizeof(actor_system_t));
  actor_system_init(a_system);

  actor_t* actor = malloc(sizeof(actor_t));
  actor_init(actor, pool, role, a_system);
  actor->id = 123;

  actor_id_t* prev = malloc(sizeof(actor_id_t));
  *prev = 420;
  message_t* message = malloc(sizeof(message_t));
  message->message_type = MSG_HELLO;
  message->data = &prev;
  message->nbytes = sizeof(actor_id_t);
  actor_push_message(actor, message);
}