#include <stdlib.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

extern const char* const queueName;

unsigned long long id = 0;
int quit = 0;

void SIGTERMHandler()
{
  quit = 1;
}

int printError_msg(const char* where)
{
  printf("ERROR (msg_handler): %s - %d\n", where, errno);
  return EXIT_FAILURE;
}

int message_handler()
{
  setbuf(stdout, NULL);

  signal(SIGTERM, SIGTERMHandler);

  mqd_t queue = mq_open(queueName, O_RDONLY);

  if (queue == -1) {
    return printError_msg("queue creation");
  }

  char buf[256];

  while (!quit) {
    ssize_t len = mq_receive(queue, buf, sizeof(buf), NULL);

    if (len == -1) {
      if (errno == EINTR) {
        continue;
      }

      sleep(1);

      printError_msg("message reception");
      continue;
    }

    printf("Order #%llu: %s\n", ++id, buf);
  }

  return EXIT_SUCCESS;
}
