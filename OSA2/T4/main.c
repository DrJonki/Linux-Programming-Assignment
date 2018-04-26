// P2, T1 - with system logging

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <syslog.h>

void SIGALRMHandler(int signum)
{
  exit(EXIT_SUCCESS);
}

int main()
{
  openlog("slog", LOG_PID | LOG_CONS, LOG_USER);

  signal(SIGALRM, SIGALRMHandler);

  alarm(30);

  for (int seconds = 1;; ++seconds) {
    sleep(1);

    syslog(LOG_INFO, "Elapsed: %d\n", seconds);
  }

  closelog();

  return EXIT_SUCCESS;
}