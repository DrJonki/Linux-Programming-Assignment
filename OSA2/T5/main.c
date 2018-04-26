#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void print_thread_id()
{
  printf("Thread id: %lu\n", pthread_self());
}

void* thread_func(void* arg)
{
  print_thread_id();

  return NULL;
}

int main()
{
  const int numThreads = 5;
  pthread_t threads[numThreads];

  printf("Spawning %d threads\n", numThreads);

  for (int i = 0; i < numThreads; ++i) {
    pthread_create(&threads[i], NULL, thread_func, NULL);
  }

  for (int i = 0; i < numThreads; ++i) {
    pthread_join(threads[i], NULL);
  }

  printf("Done\n");

  return EXIT_SUCCESS;
}