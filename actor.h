#ifndef ACTOR_H
#define ACTOR_H

#include "cacti.h"
#include "queue.h"

typedef long actor_id_t;
typedef struct actor_system actor_system_t;
typedef struct role role_t;
typedef struct message message_t;

typedef struct actor 
{
  actor_id_t id;

  actor_system_t* a_system;
  thread_pool_t* pool;
  role_t* role;

  pthread_mutex_t lock;

  queue_t* message_queue;
  bool scheduled;
  bool dead;

} actor_t;

int actor_init(actor_t* actor, thread_pool_t* pool, role_t* role, actor_system_t* a_system);

void actor_destroy(actor_t* actor);

void actor_process_message(actor_t* actor,  __attribute__ ((unused)) size_t argsz);

int actor_push_message(actor_t* actor, message_t* message);


#endif // ACTOR_H