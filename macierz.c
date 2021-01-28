#include <unistd.h>
#include "cacti.h"

#define MESSAGES          4

#define MSG_HELLO_BACK    1
#define MSG_ASSIGN_VALUES 2
#define MSG_SUM_ROW       3

typedef struct local_data {

  role_t* role;

  int n;
  int k;

  int column_id;
  int** matrix_transpose;
  int** time_transpose;

  actor_id_t first_actor;
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
  if (my_id == NULL) {
    syserr("malloc");
  }

  *my_id = actor_id_self();

  message_t msg_hello_back = {
    .message_type = MSG_HELLO_BACK,
    .data = my_id
  };

  send_message(*prev_actor, msg_hello_back);
}


void matrix_hello_back(void **stateptr, size_t nbytes, void* data) {
  printf("matrix_sum_row: Hello from actor %lu\n", actor_id_self());

  actor_id_t* next_id = (actor_id_t*)data;
  local_data_t* local_data = *stateptr;

  local_data_t* next_data = malloc(sizeof(local_data_t));
  if (next_data == NULL) {
    syserr("malloc");
  }
  // copy content of actor to next actor
  *next_data = *local_data;
  next_data->column_id++;

  message_t msg_assign_values = {
    .message_type = MSG_ASSIGN_VALUES,
    .data = local_data
  };
  send_message(*next_id, msg_assign_values);
  free(data);
}


void matrix_assign_values(void **stateptr, size_t nbytes, void* data) {
  printf("matrix_assign_values: Hello from actor %lu\n", actor_id_self());

  local_data_t* local_data = data;
  *stateptr = local_data;

  if (local_data->column_id == local_data->k - 1) {
    // last actor
    local_data->counted_rows = 0;
    local_data->row_sums = malloc(local_data->n * sizeof(int));
    if (local_data->row_sums == NULL) {
      syserr("malloc");
    }
    // start counting
    for (int i = 0; i < local_data->n; i++) {
      sum_data_t* sum_data = malloc(sizeof(sum_data_t));
      if (sum_data == NULL) {
        syserr("malloc");
      }

      sum_data->sum = 0;
      sum_data->row = i;

      message_t msg_sum_row = {
        .message_type = MSG_SUM_ROW,
        .data = sum_data
      };
      send_message(local_data->first_actor, msg_sum_row);
    }
  }
  else {
    // spawn next actor
    message_t msg_spawn = {
      .message_type = MSG_SPAWN,
      .data = local_data->role
    };
    send_message(local_data->next_actor, msg_spawn);
  }
}


void matrix_sum_row(void **stateptr, size_t nbytes, void* data) {
  printf("matrix_sum_row: Hello from actor %lu", actor_id_self());

  sum_data_t* sum_data = data;
  local_data_t* local_data = *stateptr;

  // calculate sum and wait
  sum_data->sum += local_data->matrix_transpose[local_data->column_id][sum_data->row];
  usleep(1000 * local_data->time_transpose[local_data->column_id][sum_data->row]);

  if (local_data->column_id == local_data->k - 1) {
    // last column
    local_data->row_sums[local_data->column_id] = sum_data->sum;
    local_data->counted_rows++;
    if (local_data->counted_rows == local_data->n) {
      // print output
      for (int i = 0; i < local_data->n; i++) {
        printf("%d\n", local_data->row_sums[i]);
      }
      // todo clear all actors
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
  actor_id_t first_actor;

  // role
  const size_t nprompts = MESSAGES;
  void (**prompts)(void **, size_t, void *) = malloc(sizeof(void *) * nprompts);
  prompts[0] =                 &matrix_hello;
  prompts[MSG_HELLO_BACK] =    &matrix_hello_back;
  prompts[MSG_ASSIGN_VALUES] = &matrix_assign_values;
  prompts[MSG_SUM_ROW] =       &matrix_sum_row;
  role_t role = {
    .nprompts = nprompts,
    .prompts = prompts
  };

  // create first actor
  actor_system_create(&first_actor, &role);

  int** matrix_transpose = malloc(k * sizeof(int));
  if (matrix_transpose == NULL) {
    syserr("malloc");
  }
  for (int i = 0; i < k; i++) {
    matrix_transpose[i] = malloc(n * sizeof(int));
    if(matrix_transpose[i] == NULL) {
      syserr("malloc");
    }
  }
  int** time_transpose = malloc(k * sizeof(int));
  if (time_transpose == NULL) {
    syserr("malloc");
  }
  for (int i = 0; i < k; i++) {
    time_transpose[i] = malloc(n * sizeof(int));
    if(time_transpose[i] == NULL) {
      syserr("malloc");
    }
  }

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < k; j++) {
      scanf("%d%d", &matrix_transpose[j][i], &time_transpose[j][i]);
    }
  }

  // assign values to first actor
  local_data_t* first_data = malloc(sizeof(local_data_t));
  if (first_data == NULL) {
    syserr("malloc");
  }
  first_data->role = &role;
  first_data->n = n;
  first_data->k = k;
  first_data->column_id = 0;
  first_data->matrix_transpose = matrix_transpose;
  first_data->time_transpose = time_transpose;
  first_data->first_actor = first_actor;

  message_t msg_assign_values = {
    .message_type = MSG_ASSIGN_VALUES,
    .data = first_data
  };
  send_message(first_actor,msg_assign_values);
  
}