#include "queue.h"

/* ---- queue ---- */
queue_t *new_queue(void) {

  queue_t *queue = (queue_t*)malloc(sizeof(queue_t));
  if(queue == NULL) {
    return NULL;
  }

  queue->front = NULL;
  queue->back = NULL;
  queue->length = 0;

  return queue;
}

int queue_push(queue_t *q, void *data) {
  node_t *node = (node_t*)malloc(sizeof(node_t));
  if(node == NULL) {
    return -1;
  }

  node->data = data;

  if(q->length == 0) {
    q->front = node;
    q->back = node;
  }
  else {
    q->front->prev = node;
    q->front = node;
  }

  q->length++;

  return 0;
}

void *queue_pop(queue_t *q) {
  node_t *ret = q->back->data;

  node_t *back = q->back;

  if(q->length > 1) {
    q->back = q->back->prev;
  }

  free(back);

  q->length--;
  return ret;
}

void queue_destroy(queue_t *q) {
  while(q->length > 0) {
    queue_pop(q);
  }
  free(q);
}
