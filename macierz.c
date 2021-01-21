#include "cacti.h"

int main() {
	actor_id_t first;
	role_t role;
	role.nprompts = 0;

	int res = actor_system_create(&first, &role);
	if (res == -1) {
		printf("Error in actor_system_create");
	}

	message_t message;
	message.message_type = MSG_SPAWN;
	message.data = &role;
	message.nbytes = sizeof(role_t*);

	send_message(first, message);
}
