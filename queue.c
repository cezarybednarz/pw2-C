#include "queue.h"

// max_length == 0 means there is no max length
queue_t *new_queue(size_t base_capacity, size_t max_length) {
  queue_t *queue = (queue_t*)malloc(sizeof(queue_t));
  if (queue == NULL) {
    return NULL;
  }

  queue->front = 0;
  queue->back = 0;
  queue->capacity = base_capacity;
  queue->data_array = malloc(queue->capacity * sizeof(void*));
  queue->max_length = max_length;

  if (queue->data_array == NULL) {
    return NULL;
  }
  queue->length = 0;

  return queue;
}

int queue_push(queue_t *q, void *data) {

  if (q->max_length != 0 && q->length >= q->max_length) {
    return -1;
  }

  if (q->capacity == q->length) {
    q->capacity *= 2;

    void** new_array = malloc(q->capacity * sizeof(void*));
    if (q->data_array == NULL) {
      return -1;
    }

    for (size_t i = 0; i < q->length; i++) {
      new_array[i] = q->data_array[(q->back + i) % q->length];
    }
    free(q->data_array);

    q->data_array = new_array;
    q->back = 0;
    q->front = q->length;
  }

  q->length++;
  q->data_array[q->front] = data;
  q->front = (q->front + 1) % q->capacity;

  return 0;
}

void* queue_pop(queue_t *q) {
  q->length--;
  void* ret = q->data_array[q->back];
  q->back = (q->back + 1) % q->capacity;

  return ret;
}

void queue_destroy(queue_t *q) {
  free(q->data_array);
  free(q);
}
