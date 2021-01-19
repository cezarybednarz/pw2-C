#include "cacti.h"
#include <stdio.h>

#define mxn (size_t)1e6

int rr(int a, int b) {
    return rand() % (b - a + 1) + a;
}

int main() {
    int tab[mxn];
    int pocz = 0;
    int kon = 0;

	queue_t* q = new_queue();	

	srand(time(NULL));

    int ile = rr(4, 1000000);

    size_t roz = 0;
    for (int i = 0; i < ile; i++) {
        int op = rr(1, 2);
        if (q->length == 0) {
            op = 1;
        }

        if (op == 1) {
            roz++;
            int el = rr(1, 10);
            tab[pocz++] = el;
            queue_push(q, &tab[pocz-1]);
        } 
        else {
            roz--;
            int el = *(int*)queue_pop(q);
            int pul = tab[kon++];
            if (el != pul) {
                printf("ERROR! wrong element pulled %d != %d, operation %d\n", pul, el, i);
                queue_destroy(q);
                return 0;
            }
        }
        

        if(roz != q->length) {
            printf("ERROR! wrong size (%zu != %zu)\n", roz, q->length);
            queue_destroy(q);
            return 0;
        }
    }

    queue_destroy(q);
}