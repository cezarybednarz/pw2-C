#include <unistd.h> // todo potencjalnie usunac
#include "cacti.h"

bool test_thread_pool_empty() {
	thread_pool_t* pool = malloc(sizeof(thread_pool_t));
	thread_pool_init(pool, 3);
	thread_pool_destroy(pool);
	free(pool);

	return true;
}

void counter(int* size, size_t argsz) {
	printf("argsz = %d\n", (int)argsz);
	for (int i = 0; i < *size; i++) {
		printf(" [%d]\n", i);
		usleep((rand() % 3)*1000);
	}
	printf("finished\n");
}

bool test_thread_pool_basic() {
	thread_pool_t* pool = malloc(sizeof(thread_pool_t));
	thread_pool_init(pool, 1);

	int x = 10;
	runnable_t runnable;
	runnable.arg = (int*)&x;
	runnable.argsz = sizeof(int);
	runnable.function = (void (*)(void *, size_t))&counter;

	defer(pool, runnable);
	defer(pool, runnable);
	defer(pool, runnable);
	defer(pool, runnable);

	thread_pool_destroy(pool);
	free(pool);

	return true;
}

bool test_spawn() {
	actor_id_t first;
	role_t* role = malloc(sizeof(role_t));
	role->nprompts = 0;

	int res = actor_system_create(&first, role);
	if (res != 0) {
		printf("actor_system_create failed\n");
		return false;
	}
	message_t message;
	message.message_type = MSG_SPAWN;
	message.data = role;
	message.nbytes = sizeof(role_t*);

	res = send_message(first, message);
  if (res != 0) {
    printf("error in send_message: %d\n", res);
    return false;
  }

  // todo write actor_system_join
  sleep(3);
  
	return true;
}

int main() {
   //sleep(1);
/*	if (!test_thread_pool_empty()) {
		printf("Error in test_thread_pool_empty\n");
	}
	if (!test_thread_pool_basic()) {
		printf("Error in test_thread_pool_basic\n");
	}*/
	if (!test_spawn()) {
		printf("Error in test_spawn\n");
	}
}
