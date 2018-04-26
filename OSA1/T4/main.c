#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
  pid_t pid = fork();

  if (pid <= -1) {
    return EXIT_FAILURE;
  } else if (pid == 0) {
    execl("/bin/ping", "/bin/ping", "google.com", "-c 5", (char*)0);

    exit(EXIT_SUCCESS);
  } else {
    printf("Waiting for ping to complete...\n");
  }

  wait(NULL);

  printf("Done\n");

  return EXIT_SUCCESS;
}