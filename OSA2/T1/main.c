#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void SIGINTHandler(int signum)
{
  exit(EXIT_SUCCESS);
}

int main()
{
  signal(SIGINT, SIGINTHandler);

  alarm(30);

  for (int seconds = 1;; ++seconds) {
    sleep(1);

    printf("Elapsed: %d\n", seconds);
  }

  return EXIT_SUCCESS;
}