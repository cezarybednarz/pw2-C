#ifndef ACTOR_H
#define ACTOR_H

#include "cacti.h"
#include "queue.h"
#include "threadpool.h"

typedef struct actor 
{
  actor_id_t id;

  thread_pool_t* pool;
  role_t* role;

  pthread_mutex_t* queue_lock;
  queue_t* message_queue;

  pthread_mutex_t* currently_working_lock;



} actor_t;

int actor_init(actor_t* actor, thread_pool_t* pool, role_t* role);

void actor_destroy(actor_t* actor);

int actor_receive_message(actor_t* actor, message_t* message);


#endif // ACTOR_H