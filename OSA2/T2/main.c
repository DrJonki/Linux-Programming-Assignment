#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int counter = 1;

void SIGINTHandler(int signum)
{
  int sum = 0;
  for (; counter > 0; --counter) {
    sum += counter;
  }

  printf("Sum: %d\n", sum);

  exit(EXIT_SUCCESS);
}

int main()
{
  signal(SIGINT, SIGINTHandler);

  for (;; ++counter) {
    printf("Counter: %d\n", counter);

    sleep(1);
  }

  return EXIT_SUCCESS;
}
