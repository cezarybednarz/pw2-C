#ifndef ACTOR_SYSTEM_H
#define ACTOR_SYSTEM_H

#include "cacti.h"

typedef long actor_id_t;
typedef struct actor actor_t;


typedef struct actor_system
{
  pthread_mutex_t lock;

  thread_pool_t* pool;
  queue_t* actors;

} actor_system_t;


int actor_system_init(actor_system_t* a_system);

void actor_system_destroy(actor_system_t* a_system);

actor_t* actor_system_find(actor_system_t* a_system, actor_id_t id);

int actor_system_insert(actor_system_t* a_system, actor_t* actor);

#endif // ACT0R_SYSTEM_H