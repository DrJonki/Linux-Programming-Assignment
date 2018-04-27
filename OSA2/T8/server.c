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
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    return printError("socket creation");
  }

  {
    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8080);

    if (bind(server_socket, (struct sockaddr*)&server, sizeof(server)) == -1) {
      return printError("socket bind");
    }
  }

  if (listen(server_socket, 1) == -1) {
    return printError("socket listen");
  }

  printf("Listening for connection\n");

  {
    struct sockaddr_in client;
    socklen_t len;

    int client_socket = accept(server_socket, (struct sockaddr*)&client, &len);

    if (client_socket == -1) {
      return printError("accept client");
    }

    printf("Client connected\n");

    while (1) {
      char buf[1024];
      ssize_t received = recv(client_socket, buf, sizeof(buf), 0);

      if (received == -1) {
        return printError("socket receive");
        break;
      }

      if (received == 0) {
        printf("Client disconnected\n");
        break;
      } else {
        printf("Request received: %s\n", buf);

        int left, right;
        sscanf(buf, "%d %d", &left, &right);

        int messageLen = sprintf(buf,
          "SUM: %d\nSUBTRACT: %d\nPRODUCT: %d",
          left + right, left - right, left * right
        );

        ssize_t written = write(client_socket, buf, messageLen + 1);

        if (written == -1) {
          return printError("client write");
        }
      }
    }
  }

  return EXIT_SUCCESS;
}