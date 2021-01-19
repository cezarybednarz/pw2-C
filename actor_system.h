#ifndef ACTOR_SYSTEM_H
#define ACTOR_SYSTEM_H

#include "threadpool.h"
#include "cacti.h"
#include "queue.h"

typedef struct actor_system 
{
  thread_pool_t* pool;
  role_t* role;

} actor_system_t;

int actor_system_init(actor_system_t* actor_system);

void actor_system_destroy(actor_system_t* actor_system);

#endif // ACT0R_SYSTEM_H