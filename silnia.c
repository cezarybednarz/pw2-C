#include "cacti.h"

#define MESSAGES          3

#define MSG_HELLO_BACK    1
#define MSG_SEND_VALUE    2

typedef struct value_data {
  int curr;
  int n;
  int result;
  role_t* role;
} value_data_t;


void silnia_hello(void **stateptr __attribute__((unused)), size_t nbytes __attribute__((unused)), void* data) {

  printf("silnia_hello: \n");
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

void silnia_hello_back(void **stateptr __attribute__((unused)), size_t nbytes __attribute__((unused)), void* data) {
  value_data_t* value_data = *stateptr;
  actor_id_t next_actor = *(actor_id_t*)data;
  free(data);

  printf("silnia_hello_back: value_data->curr = %d, value_data->result = %d, value_data->n = %d", value_data->curr, value_data->result, value_data->n);

  value_data->curr++;
  value_data->result *= value_data->curr;

  if (value_data->curr == value_data->n) {
    printf("%d\n", value_data->result);
    return;
  }

  message_t msg_send_value = {
    .message_type = MSG_SEND_VALUE,
    .data = value_data
  };
  send_message(next_actor, msg_send_value);
}

void silnia_send_value(void **stateptr, size_t nbytes __attribute__((unused)), void* data) {
  value_data_t* value_data = data;
  *stateptr = value_data;


  message_t msg_spawn = {
    .message_type = MSG_SPAWN,
    .data = value_data->role
  };
  send_message(actor_id_self(), msg_spawn);
}

int main() {
  int n;
  scanf("%d", &n);

  actor_id_t first_actor;

  // role
  const size_t nprompts = MESSAGES;
  void (**prompts)(void **, size_t, void *) = malloc(sizeof(void *) * nprompts);
  prompts[0] =                 &silnia_hello;
  prompts[MSG_HELLO_BACK] =    &silnia_hello_back;
  prompts[MSG_SEND_VALUE] =    &silnia_send_value;
  role_t role = {
    .nprompts = nprompts,
    .prompts = prompts
  };

  // create first actor
  actor_system_create(&first_actor, &role);

  value_data_t* first_data = malloc(sizeof(value_data_t));
  if (first_data == NULL) {
    syserr("malloc");
  }

  first_data->n = n;
  first_data->curr = 0;
  first_data->result = 1;
  first_data->role = &role;

  message_t msg_send_value = {
    .message_type = MSG_SEND_VALUE,
    .data = first_data
  };
  send_message(first_actor, msg_send_value);

  actor_system_join(first_actor);

  free(first_data);
  return 0;
}
