#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
  srand(atoi(argv[1]));

  int n = 1000;
  int k = 100;
  printf("%d\n%d\n", n, k);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < k; j++) {
      printf("%d %d\n", 1, 0);
    }
  }
}