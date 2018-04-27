#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

int printError(const char* where)
{
  printf("ERROR: %s - %d\n", where, errno);
  return EXIT_FAILURE;
}

int make_connection(const uint16_t port)
{
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock == -1) {
    printError("socket creation");
    return -1;
  }

  struct sockaddr_in server;

  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  server.sin_family = AF_INET;
  server.sin_port = port;

  if (connect(sock, (struct sockaddr*)&server, (socklen_t)sizeof(server)) == -1) {
    printError("socket connect");
    return -1;
  }

  return sock;
}

int main()
{
  srand(time(NULL));

  int sock;
  uint16_t port;

  // Make connection to fetch the port
  {
    sock = make_connection(50000);

    if (sock == -1) {
      return EXIT_FAILURE;
    }

    char buf[10];
    ssize_t received = recv(sock, buf, sizeof(buf), 0);

    if (received == -1) {
      return printError("first recv");
    }

    port = (uint16_t)strtoul(buf, NULL, 10);

    printf("Got port %u\n", port);

    close(sock);
  }

  // 3 connection attempts
  for (int i = 0; i < 3; ++i) {
    printf("Connecting to port %u\n", port);

    sock = make_connection(port);

    if (sock == -1) {
      sleep(1);
      continue;
    }

    printf("Connected\n");

    break;
  }

  if (sock == -1) {
    return EXIT_FAILURE;
  }

  const char* dishes[] = {
    "Baguette", "Croissant", "Creme Brulee", "Macarons", "Souffle", "Crepe et fruit",
    "Fuseau lorrain", "Tourte", "Presskopf", "Confit de canard", "Andouillettes", "Coq au vin",
    "Raclette", "Salade Aveyronaise", "Rouille de seiche", "Ratatouille", "Fougasse"
  };

  for (int i = rand() % 3; i < 5; ++i) {
    int index = rand() % (sizeof(dishes) / sizeof(const char*) - 1);

    ssize_t sent = send(sock, dishes[index], strlen(dishes[index]) + 1, 0);

    if (sent == -1) {
      return printError("socket send");
    }

    printf("Sent order: %s\n", dishes[index]);

    sleep(1);
  }

  close(sock);

  printf("Disconnected\n");

  return EXIT_SUCCESS;
}