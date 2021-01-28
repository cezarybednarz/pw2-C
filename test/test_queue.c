#include "../cacti.h"
#include <stdio.h>

#define mxn (size_t)1e3

typedef struct info
{
  long a;
  long b;
  long c;
} info_t;

int rr(int a, int b) {
    return rand() % (b - a + 1) + a;
}

int main() {

  info_t *test = malloc(sizeof(info_t));
  queue_t* test_q = new_queue();
  queue_push(test_q, test);
  info_t* out = (info_t*)queue_pop(test_q);
  out->a = 69;
  out->b = 420;
  out->c = 3;
  if (out->a != 69 || out->b != 420) {
    printf("error!\n");
  }
  free(test);
  queue_destroy(test_q);


  info_t tab[mxn];
  int pocz = 0;
  int kon = 0;

	queue_t* q = new_queue();	

	srand(time(NULL));

    int ile = rr(4, 100);

    size_t roz = 0;
    for (int i = 0; i < ile; i++) {
        int op = rr(1, 2);
        if (q->length == 0) {
            op = 1;
        }

        if (op == 1) {
            roz++;
            int el = rr(1, 1000);
            tab[pocz].a = el;
            tab[pocz].b = el + 1;
            pocz++;

            queue_push(q, &tab[pocz-1]);
        } 
        else {
            roz--;
            info_t el = *(info_t*)queue_pop(q);
            info_t pul = tab[kon++];
            if (el.a != pul.a || el.b != pul.b) {
                printf("ERROR! wrong element pulled %ld != %ld or %ld != %ld, operation %d\n", pul.a, el.a, pul.b, el.b, i);
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

    printf("OK\n");
}