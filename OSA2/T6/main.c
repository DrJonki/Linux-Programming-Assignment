#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>

int main()
{
  sem_t semaphore;
  sem_init(&semaphore, 0, 0);
  int quit = 0;

  while (!quit) {
    printf("1 - print semaphore value\n");
    printf("2 - decrement semaphore value\n");
    printf("3 - increment semaphore value\n");
    printf("4 - quit\n");

    printf("Input: ");

    int answer;
    scanf("%d", &answer);

    printf("\n");

    switch (answer) {
      case 1: {
        int value;
        sem_getvalue(&semaphore, &value);

        printf("Semaphore value: %d", value);

        break;
      }
      case 2: {
        sem_trywait(&semaphore);

        printf("Done");

        break;
      }
      case 3: {
        sem_post(&semaphore);

        printf("Done");

        break;
      }
      case 4: {
        quit = 1;

        break;
      }
      default: {
        printf("Invalid input");
      }
    }

    printf("\n\n");
  }

  sem_destroy(&semaphore);

  return EXIT_SUCCESS;
}