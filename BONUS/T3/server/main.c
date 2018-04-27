#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <mqueue.h>
#include <signal.h>

#ifndef MAX_CONNECTIONS
  #define MAX_CONNECTIONS 10
#endif

#if MAX_CONNECTIONS <= 1
  #error Invalid value for MAX_CONNECTIONS (must be positive)
#endif

extern int init_connection_pool();
extern in_port_t prepare_new_connection();
extern int message_handler();

const char* const queueName = "/bonus_t3_mq";

int printError(const char* where)
{
  printf("ERROR (main): %s - %d\n", where, errno);
  return EXIT_FAILURE;
}

int main()
{
  // Initialize the message queue
  {
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 256;
    attr.mq_curmsgs = 0;

    mqd_t queue = mq_open(queueName, O_CREAT | O_RDONLY, 0660, &attr);

    if (queue == -1) {
      return printError("queue creation");
    }
  }

  pid_t msg_handler;

  // Fork the message handler process
  {
    msg_handler = fork();

    if (msg_handler == -1) {
      return printError("fork");
    }

    if (msg_handler == 0) {
      exit(message_handler());
    }
  }

  if (init_connection_pool() != 0) {
    return EXIT_FAILURE;
  }

  // Create main socket & start listening for connections
  {
    struct sockaddr_in server;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = 50000;

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == -1) {
      return printError("main socket creation");
    }

    if (bind(sock, (struct sockaddr*)&server, sizeof(server)) == -1) {
      return printError("main socket bind");
    }

    if (listen(sock, MAX_CONNECTIONS) == -1) {
      return printError("main socket listen");
    }

    printf("Listening for clients\n\n");

    while (1) {
      struct sockaddr_in client;
      socklen_t addrlen = sizeof(client);
      int client_sock = accept(sock, (struct sockaddr*)&client, &addrlen);

      if (client_sock == -1) {
        return printError("main socket accept");
      }

      in_port_t port = prepare_new_connection();

      if (port == 0) {
        return EXIT_FAILURE;
      }

      char portbuf[10];
      int len = sprintf(portbuf, "%d", port);

      if (write(client_sock, portbuf, len + 1) == -1) {
        return printError("client port send");
      }

      close(client_sock);
    }
  }

  kill(msg_handler, SIGTERM);
  wait(NULL);

  return EXIT_SUCCESS;
}