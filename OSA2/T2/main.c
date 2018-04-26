#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutex;
int counter = 1;

void SIGINTHandler(int signum)
{
  pthread_mutex_lock(&mutex);

  int sum = 0;
  for (; counter > 0; --counter) {
    sum += counter;
  }

  printf("Sum: %d\n", sum);

  pthread_mutex_unlock(&mutex);

  exit(EXIT_SUCCESS);
}

int main()
{
  pthread_mutex_init(&mutex, NULL);

  signal(SIGINT, SIGINTHandler);

  for (;; ++counter) {
    pthread_mutex_lock(&mutex);

    printf("Counter: %d\n", counter);

    pthread_mutex_unlock(&mutex);

    sleep(1);
  }

  return EXIT_SUCCESS;
}
