#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int printError(const char* where)
{
  printf("ERROR: %s - %d\n", where, errno);
  return EXIT_FAILURE;
}

int main()
{
  int client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket == -1) {
    return printError("socket creation");
  }

  {
    struct sockaddr_in server;

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);

    if (connect(client_socket, (struct sockaddr*)&server, sizeof(server)) == -1) {
      return printError("socket connect");
    }

    printf("Connected\n");
  }

  char buf[1024];

  while (1) {
    printf("Enter 2 integers: ");

    int left, right;
    if (scanf("%d %d", &left, &right) != 2) {
      printf("\n\nInvalid input\n\n");

      // Clear input buffer to prevent infinete loop
      while (getchar() != '\n');
      
      continue;
    }

    int messageLen = sprintf(buf, "%d %d", left, right);

    ssize_t sent = send(client_socket, buf, messageLen + 1, 0);

    if (sent == -1) {
      return printError("socket send");
    }

    ssize_t received = recv(client_socket, buf, 1024, 0);

    if (received == -1) {
      return printError("socket receive");
    }

    printf("%s\n\n", buf);
  }

  return EXIT_SUCCESS;
}