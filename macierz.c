#include "cacti.h"

#include <stdio.h>

int main() {
	queue_t* q = new_queue();

	int val = 3;
	queue_push(q, &val);
	int* wyn = (int*)queue_pop(q);
	queue_destroy(q);

	printf("%d\n", *wyn);



	return 0;
}
