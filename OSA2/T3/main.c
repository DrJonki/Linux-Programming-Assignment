#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

typedef struct
{
  pid_t pid;
  int terminated;
} child_info;

child_info chld_info[10];

void SIGCHLDHandler(int signum)
{
  while (1) {
    pid_t childPid = wait(NULL);

    if (childPid <= 0) {
      break;
    }

    for (int i = 0; i < 10; ++i) {
      child_info* info = &chld_info[i];

      if (info->pid == childPid) {
        printf("Child %d terminated\n", childPid);

        info->terminated = 1;
        break;
      }
    }
  }
}

int main()
{
  setbuf(stdout, NULL);

  srand(time(NULL));

  signal(SIGCHLD, SIGCHLDHandler);

  for (int i = 0; i < 10; ++i) {
    pid_t pid = fork();
    int seconds = (rand() % 5) + 1;

    if (pid <= -1) {
      return EXIT_FAILURE;
    }
    else if (pid == 0) {
      // Child

      // Sleep for 1-5 seconds
      sleep(seconds);

      exit(EXIT_SUCCESS);
    } else {
      child_info* info = &chld_info[i];

      info->pid = pid;
      info->terminated = 0;
    }
  }

  printf("Spawned children\n");

  while (1) {
    int done = 1;

    for (int i = 0; i < 10; ++i) {
      if (!chld_info[i].terminated) {
        done = 0;
        break;
      }
    }

    if (done) {
      break;
    }

    usleep(500 * 1000); // 500ms
  }

  printf("Done\n");

  return EXIT_SUCCESS;
}