/* zombie test (corrected) */
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
  pid_t pid;
  
  switch (pid = fork())
  {
    case -1:
      perror("fork failed");
      exit(1);
    case 0:
      printf("CHILD: My PID is %d, My parent's PID is %d\n", getpid(), getppid());
      exit(0);
    default:
      printf("PARENT: My PID is %d, My child's PID is %d\n", getpid(), pid);
      //printf("PARENT: I'm now looping...\n");while(1);

      // Equivalent to: waitpid(-1, NULL, 0);
      // There's just one child process, so we can just use wait() here.
      wait(NULL);
  }

  exit(0);
}

/*

Zombie processes happen when a child process terminates and the parent doesn't handle this state
change. This leads to the situation where the resources used by the child process are not being
released.

A zombie process is one whose termination has not been handled by its parent. Orphan process is
a running child process whose parent process has been terminated.

A return statement will return from the current function, while exit() will stop program
execution regardless of where it's called from. Returning from main() does pretty much the
same thing as calling exit().

*/