#include "cacti.h"

#define MESSAGES          4

#define MSG_HELLO_BACK    1
#define MSG_ASSIGN_VALUE  2
#define MSG_KILL          3

typedef struct value_data {
  int curr;
  int n;
  int result;

  actor_id_t first_actor;
  actor_id_t next_actor;

  role_t* role;
} value_data_t;


void silnia_hello(void **stateptr __attribute__((unused)), size_t nbytes __attribute__((unused)), void* data) {
  actor_id_t prev_actor = (actor_id_t)data;

  if (prev_actor == actor_id_self()) {
    // first actor from system
    return;
  }

  actor_id_t* my_id = malloc(sizeof(actor_id_t));
  if (my_id == NULL) {
    syserr("malloc");
  }

  *my_id = actor_id_self();

  message_t msg_hello_back = {
    .message_type = MSG_HELLO_BACK,
    .data = my_id
  };

  send_message(prev_actor, msg_hello_back);
}

void silnia_hello_back(void **stateptr __attribute__((unused)), size_t nbytes __attribute__((unused)), void* data) {
  value_data_t* value_data = *stateptr;
  actor_id_t next_actor = *(actor_id_t*)data;
  free(data);

  value_data_t* next_data = malloc(sizeof(value_data_t));
  if (next_data == NULL) {
    syserr("malloc");
  }
  *next_data = *value_data;
  next_data->curr++;
  value_data->next_actor = next_actor;

  message_t msg_assign_value = {
    .message_type = MSG_ASSIGN_VALUE,
    .data = next_data
  };
  send_message(next_actor, msg_assign_value);
}

void silnia_assign_value(void **stateptr, size_t nbytes __attribute__((unused)), void* data) {
  *stateptr = data;
  value_data_t* value_data = *stateptr;

  value_data->result *= value_data->curr;

  if (value_data->curr == value_data->n) {

    printf("%d\n", value_data->result);

    message_t msg_kill = {
      .message_type = MSG_KILL,
    };
    send_message(value_data->first_actor, msg_kill);
  }
  else {
    message_t msg_spawn = {
      .message_type = MSG_SPAWN,
      .data = value_data->role
    };
    send_message(actor_id_self(), msg_spawn);
  }
}

void silnia_kill(void **stateptr, size_t nbytes __attribute__((unused)), void* data __attribute__((unused))) {
  value_data_t* value_data = *stateptr;

  actor_id_t next_actor = value_data->next_actor;
  bool last = value_data->curr == value_data->n;

  free(value_data);

  message_t msg_godie = {
    .message_type = MSG_GODIE
  };
  send_message(actor_id_self(), msg_godie);

  if (!last) {
    message_t msg_kill = {
      .message_type = MSG_KILL
    };
    send_message(next_actor, msg_kill);
  }
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
  prompts[MSG_ASSIGN_VALUE] =  &silnia_assign_value;
  prompts[MSG_KILL] =          &silnia_kill;
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
  first_data->curr = 1;
  first_data->result = 1;
  first_data->role = &role;
  first_data->first_actor = first_actor;

  message_t msg_assign_value = {
    .message_type = MSG_ASSIGN_VALUE,
    .data = first_data
  };
  send_message(first_actor, msg_assign_value);

  actor_system_join(first_actor);

  free(prompts);
  return 0;
}
