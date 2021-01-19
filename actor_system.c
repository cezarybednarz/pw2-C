#include "actor_system.h"

int actor_system_init(actor_system_t* actor_system) {
  actor_system = (actor_system_t*)malloc(sizeof(actor_system_t));
  if (actor_system == NULL) {
    return -1;
  }

  if (thread_pool_init(actor_system->pool, POOL_SIZE) != 0) {
    return -1;
  }


  return 0;
}



void actor_system_destroy(actor_system_t* actor_system) {
  thread_pool_destroy(actor_system->pool);
  
  free(actor_system);
}