#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

const char* const queueName = "/p1_t7_mq";

void child()
{
  mqd_t queue = mq_open(queueName, O_RDWR);

  char buf[256];

  ssize_t size = mq_receive(queue, buf, sizeof(buf), NULL);

  if (size > -1) {
    buf[size] = '\0';
    printf("Child: got message \"%s\"\n", buf);
  } else {
    return; // Error
  }

  mq_close(queue);
}

int main()
{
  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = 255;
  attr.mq_curmsgs = 0;

  mqd_t queue = mq_open(queueName, O_CREAT | O_RDWR | O_NONBLOCK, 0660, &attr);

  const char* const message = "message from parent";
  mq_send(queue, message, strlen(message), 1);

  pid_t pid = fork();

  if (pid == 0) {
    child();
    exit(EXIT_SUCCESS);
  }

  wait(NULL);

  {
    char buf[256];

    ssize_t size = mq_receive(queue, buf, sizeof(buf), NULL);

    if (size == -1 && errno == EAGAIN) {
      printf("Parent: message successfully popped by child process\n");
    } else {
      printf("Parent: message not deleted or unknown error occurred in child process\n");
    }
  }

  mq_close(queue);
  mq_unlink(queueName);

  return EXIT_SUCCESS;
}
