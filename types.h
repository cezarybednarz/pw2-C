#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>
#include "threadpool.h"
#include "queue.h"
#include "cacti.h"

typedef long actor_id_t;


typedef struct actor_system 
{
  pthread_mutex_t* lock;

  thread_pool_t* pool;
  queue_t* actors;

} actor_system_t;

typedef struct actor 
{
  actor_id_t id;

  actor_system_t* a_system;
  thread_pool_t* pool;
  role_t* role;

  pthread_mutex_t* lock;

  queue_t* message_queue;
  bool scheduled;
  bool dead;

} actor_t;

#endif // TYPES_H