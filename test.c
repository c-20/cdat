#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  int test = system("table -count-flags @incrementreadinputoutput -c 3 == raised");
  test = WEXITSTATUS(test);
  printf("returned: %d\n", test);
  return 0;
}

