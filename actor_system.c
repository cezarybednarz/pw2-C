#include "actor_system.h"

int actor_system_init(actor_system_t* actor_system) {
  actor_system->pool = malloc(sizeof(thread_pool_t));
  if (thread_pool_init(actor_system->pool, POOL_SIZE) != 0) {
    fprintf(stderr, "thread_pool_init\n");
    return -1;
  }

  actor_system->actors = new_queue();
  if (actor_system->actors == NULL) {
    fprintf(stderr, "new_queue\n");
    return -1;
  }
  
  if (pthread_mutex_init(&(actor_system->lock), NULL) != 0) {
    fprintf(stderr, "pthread_mutex_init\n");
    return -1;
  }
  return 0;
}

void actor_system_destroy(actor_system_t* actor_system) {
  thread_pool_destroy(actor_system->pool);
  queue_destroy(actor_system->actors);  
  free(actor_system->pool);
  free(actor_system);
}

actor_t* actor_system_find(actor_system_t* a_system, actor_id_t id) {
  if (pthread_mutex_lock(&(a_system->lock)) != 0) {
    return NULL;
  }

  if ((unsigned long)id >= a_system->actors->length) {
    return NULL;
  }
  actor_t* ret = a_system->actors->data_array[id];

  if (pthread_mutex_unlock(&(a_system->lock)) != 0) {
    return NULL;
  }
  return ret;
}

int actor_system_insert(actor_system_t* a_system, actor_t* actor) {
  if (pthread_mutex_lock(&(a_system->lock)) != 0) {
    return -1;
  }

  if (queue_push(a_system->actors, actor) != 0) {
    pthread_mutex_unlock(&(a_system->lock)); 
    return -1;
  }

  actor->id = a_system->actors->length - 1;
  int ret = actor->id;

  if (pthread_mutex_unlock(&(a_system->lock)) != 0) {
    return -1;
  }
  return ret;
}

