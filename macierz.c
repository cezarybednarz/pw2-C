#include <unistd.h>
#include "cacti.h"

#define MESSAGES        4

#define MSG_HELLO_BACK  1
#define MSG_ASSIGN_NEXT 2
#define MSG_SUM_ROW     3

typedef struct local_data {
  int n;
  int k;

  int column_id;
  int** matrix_transpose;
  int** time_transpose;

  actor_id_t next_actor;

  // last column data
  int counted_rows;
  int* row_sums;
} local_data_t;

typedef struct sum_data {
  int sum;
  int row;
} sum_data_t;

void matrix_hello(void **stateptr, size_t nbytes, void* data) {
  printf("matrix_hello: Hello from actor %lu, message sent by %lu\n", actor_id_self(), *((actor_id_t*)data));

  actor_id_t* prev_actor = data;
  actor_id_t* my_id = malloc(sizeof(actor_id_t));
  *my_id = actor_id_self();

  message_t msg_hello_back = {
    .message_type = MSG_HELLO_BACK,
    .data = my_id
  };

  send_message(*prev_actor, msg_hello_back);
}

void matrix_hello_back(void **stateptr, size_t nbytes, void* data) {
  printf("matrix_sum_row: Hello from actor %lu", actor_id_self());

  actor_id_t* next_id = (actor_id_t*)data;
  local_data_t* local_data = *stateptr;

  local_data->next_actor = *next_id;
}

void matrix_assign_next(void **stateptr, size_t nbytes, void* data) {

}

void matrix_sum_row(void **stateptr, size_t nbytes, void* data) {
  printf("matrix_sum_row: Hello from actor %lu", actor_id_self());

  sum_data_t* sum_data = data;
  local_data_t* local_data = *stateptr;

  // calculate sum and wait
  sum_data->sum += local_data->column[sum_data->row];
  usleep(1000 * local_data->column_time[sum_data->row]);

  if (local_data->column_id == local_data->k - 1) {
    // last column
    local_data->row_sums[local_data->column_id] = sum_data->sum;
    local_data->counted_rows++;
    if (local_data->counted_rows == local_data->n) {
      // print output
      for (int i = 0; i < local_data->n; i++) {
        printf("%d\n", local_data->row_sums[i]);
      }
    }
  }
  else {
    // send message to next actor
    message_t msg_sum_row = {
      .message_type = MSG_SUM_ROW,
      .data = sum_data
    };
    send_message(local_data->next_actor, msg_sum_row);
  }
}

int main() {
  // input
  int n, k;
  scanf("%d%d", &n, &k);
  int sums[n];
  actor_id_t actors[k];

  for(int i = 0; i < n; i++) {
    sums[i] = 0;
  }

  // role
  const size_t nprompts = MESSAGES;
  void (**prompts)(void **, size_t, void *) = malloc(sizeof(void *) * nprompts);
  prompts[0] =               &matrix_hello;
  prompts[MSG_HELLO_BACK] =  &matrix_hello_back;
  prompts[MSG_ASSIGN_NEXT] = &matrix_assign_next;
  prompts[MSG_SUM_ROW] =     &matrix_sum_row;
  role_t role = {
    .nprompts = nprompts,
    .prompts = prompts
  };

  // create first actor
  actor_system_create(&actors[0], &role);

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < k; j++) {
      int val, time;
      scanf("%d%d", &val, &time);


    }
  }
}